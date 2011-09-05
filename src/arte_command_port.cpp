#include <stdio.h>
#include <stdint.h>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/foreach.hpp>
#include "arte_command_port.h"

//************* Simple constructor ****************/
Arte_command_port::Arte_command_port()
{
  basic_init();
}


Arte_command_port::Arte_command_port(boost::property_tree::ptree pt,
				     CALLBACK_FN cb_func, void *cb_arg,
				     bool auto_start)
{
  basic_init();
  _auto_start = auto_start;
  my_pt = new boost::property_tree::ptree( pt );
  do_pt_settings();
  set_callback_fn( cb_func, cb_arg );
  if(_auto_start)
    start();
}

Arte_command_port::Arte_command_port(std::string pt_pathname,
				     CALLBACK_FN cb_func, void *cb_arg,
				     bool auto_start)
{
  basic_init();
  _auto_start = auto_start;
  my_pt = new boost::property_tree::ptree;
  read_xml( pt_pathname, *my_pt );
  do_pt_settings();
  set_callback_fn ( cb_func, cb_arg );
  if(_auto_start)
    start();
}

//** Read hostname and port number from config file into our zmq addy strings **/
void Arte_command_port::do_pt_settings()
{
  using boost::property_tree::ptree;

  primary_port = 
    my_pt->get<std::string>("options.setup.command_port");
  secondary_port =
    my_pt->get<std::string>("options.setup.secondary_command_port");

  // Initialize all zmq strings
  std::string out_m("tcp://*:");
  out_m += primary_port;
  std::string out_s("tcp://localhost:");
  out_s += secondary_port;
  std::string in_secondary("tcp://*:");
  in_secondary += secondary_port;
  
  std::vector<std::string> in_list;

  BOOST_FOREACH(ptree::value_type &v, my_pt->get_child("options.setup.hosts")){
    std::string this_host (v.second.data());
    std::string this_in("tcp://");
    this_in += this_host;
    this_in += ":";
    this_in += primary_port;
    in_list.push_back(this_in);
  }

  set_addy_str( in_list, out_m, out_s, in_secondary );
}

//***** Zero out fields that need to be set *********/
void Arte_command_port::basic_init()
{
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  my_pt = NULL;
  pt_fn.clear();
  my_zmq_context = NULL;
  my_publisher = NULL;
  my_subscriber = NULL;
  running = false;
  reset_addy_str();
  callback_fn = NULL;
  callback_arg = NULL;
  _auto_start = false;
}


//****** Simple addr string setter *****************/
void Arte_command_port::set_addy_str( std::vector<std::string> in_str,
				      std::string& out_str_m,
				      std::string& out_str_s,
				      std::string& secondary_in)
{
  if(!in_str.empty())
    in_addy_str.insert( in_addy_str.end(), in_str.begin(), in_str.end());
  if(!out_str_m.empty())
    out_addy_str_m.assign( out_str_m.c_str());
  if(!out_str_s.empty())
     out_addy_str_s.assign( out_str_s.c_str() );
  if(!secondary_in.empty())
    secondary_in_addy_str.assign( secondary_in.c_str() );
}

void Arte_command_port::reset_addy_str()
{
  in_addy_str.clear();
  out_addy_str_m.clear();
  out_addy_str_s.clear();
  secondary_in_addy_str.clear();
}

//********* Simple callback setter *****************/
void Arte_command_port::set_callback_fn( CALLBACK_FN cb_fn, void *arg )
{
  callback_fn = cb_fn;
  callback_arg = arg;
}


//*** Initialize publisher socket and thread for ******/
//*** listening to subscriber socket ******************/
int Arte_command_port::start()
{
  if( !ok_to_start() ){
    printf("Arte_command_port couldn't start; uninitialized fields.\n");
    return 1;
  }

  my_zmq_context = new zmq::context_t(1);

  if(my_zmq_context == NULL){
    printf("zmq error creating context\n");
    return 1;
  }

  running = true;
  int rc = init_send();
  if(rc){
    printf("Arte_command_port failed to initialize publisher. rc: %d\n",rc);
    printf("out_addy_str_m was: _%s_\n", out_addy_str_m.c_str() );
    printf("Attempted to be master (1 = true): %d", (int) is_master);
    printf("Likely ok - just means we are in slave mode.\n");
  } 

  debug_print_strings(); //TODO: remove

  rc = pthread_create( &listener_thread,           /* thread_t          */
		       NULL,                       /* thread attributes */
		       &listen_in_thread_wrapper,  /* thread fn         */
		       this);                      /* arg to thread fn  */
  if(rc){
    running = false;
    printf("Arte_command_port failed to listen in thread.\n");
    return 1;
  }

}


int Arte_command_port::stop()
{
  running = false;
}

//*********** Publish an ArteCommand to the network *************/
int Arte_command_port::send_command(ArteCommand the_command)
{

  if( my_zmq_context == NULL ){
    printf("Arte_command_port: tried to send a message before calling start().\n");
    printf("Call start() first.\n");
    return 1;
  }

  std::string command_str; 
  command_str.erase();
  command_str.assign(100, '\0'); // this is needed to 'blank initialize'
                                 // without it, malformed data to zmq
  if( !(the_command.SerializeToString( &command_str ))){
    printf("Arte_command_port Serialize error on command:\n");
    the_command.PrintDebugString();
    return 1;
  }

  try{
    zmq::message_t z_msg ( command_str.size() ); // The right way according to zmq docsshe
    memcpy( z_msg.data(), command_str.c_str(), command_str.size() );
    
    my_publisher->send(z_msg);
    printf("Passed send.  is_master: %d\n", (int) is_master); //TODO: remove
  }
  catch( zmq::error_t& e ){
    printf("Arte_command::send_command couldn't create a message from the buffer.\n");
    printf("zmq_error messsage is: _%s_\n", e.what() );
    return 1;
  }

  if(is_master == false){
    zmq::message_t response;
    my_publisher->recv( &response );

    printf("Got response.  Size: %d Message: _%s_\n",   // TODO: remove
	   response.size(), (char*) response.data() );
  }

  return 0;
}

//Arte_command_port::send_command2(

Arte_command_port::~Arte_command_port()
{
  
  delete my_zmq_context;    // this was newed in start()
  delete my_publisher;  // this was newed in init_send()
  delete my_subscriber; // this was newed in listen_in_thred()
                     
}


//******* Pop the command from the front of the queue ********/
ArteCommand Arte_command_port::command_queue_pop()
{
  ArteCommand ret_val;
  ret_val = command_queue.front();
  command_queue.pop();
  return ret_val;
}


//********** Check the size of the queue****************/
int Arte_command_port::command_queue_size()
{
  return (int) command_queue.size();
}


//****** Check that necessary fields have been set *****/
bool Arte_command_port::ok_to_start()
{
  if( in_addy_str.empty())
    printf("in_addy_str is empty.\n");
  if( out_addy_str_m.empty())
    printf("out_addy_str_m is empty.\n");
  if( out_addy_str_s.empty())
    printf("out_addy_str_s is empty.\n");
  if( secondary_in_addy_str.empty())
    printf("secondary_in_addy_str is empty.\n");
  if( callback_fn == NULL )
    printf("NULL for callback_fn.\n");
  if( callback_arg == NULL )
    printf("NULL for callback arg.\n");

  return ( !(in_addy_str.empty() |
	     out_addy_str_m.empty() |
	     out_addy_str_s.empty() |
	     secondary_in_addy_str.empty() |
	     (callback_fn == NULL) | 
	     (callback_arg == NULL)) );
}


//************ Initialize the publisher socket ***************/
int Arte_command_port::init_send()
{

  // Initiate as though we are master
  my_publisher = new zmq::socket_t( *my_zmq_context, ZMQ_PUB );
  if( my_publisher == NULL ){
    printf("Arte_command_port failed to create publisher socket.\n");
    return 2;
  }
  try{
    my_publisher->bind( out_addy_str_m.c_str() );
  }
  catch(std::exception& e){
    printf("Caught an exception in Arte_command_port::init_send.\n");
    printf("what(): _%s_\n", e.what() ); fflush(stdout);
    printf("out_addy_str_m was: _%s_\n", out_addy_str_m.c_str() );
    printf("Setting this process Arte_command_port to slave mode.\n");
    is_master = false;
  }
  if(is_master == true){
    printf("Successful master mode init.\n"); // TODO: remove
    // if we are master, then sending is initialized
    return 0;
  }
  
  // but if bind on primary port failed, we must send as slave
  delete my_publisher;
  my_publisher = new zmq::socket_t( *my_zmq_context, ZMQ_REQ );
  if(my_publisher == NULL){
    printf("Arte_command_port failed to create slave REQ client.\n");
    return 2;
  }
  try{
    my_publisher->connect( out_addy_str_s.c_str() );
  }
  catch(std::exception& e){
    printf("Caught an exception in Arte_command_port::init_send\n");
    printf("while trying to connect slave REQ socket.\nwhat(): _%s_\n",
	   e.what());
    printf("out_addy_str_s was: _%s_\n", out_addy_str_s.c_str() );
    return 5;
  }
  printf("Successful slave mode init.\n"); // TODO: remove
  // if no exception, then we successful initialized slave sending
  return 0;
}


//**** Static wrapper function to use as fn ptr for thread call ****/
void* Arte_command_port::listen_in_thread_wrapper(void *arg)
{
  // Cast arg to Arte_command_port and call listen_in_thread method
  ( (Arte_command_port*)arg )->listen_in_thread();
}


//******** Start thread and do listening loop in that thread ********/
int Arte_command_port::listen_in_thread()
{
  // Both masters and slaves initialize a subscriber 
  // for primary messages
  my_subscriber = new zmq::socket_t ( *my_zmq_context, ZMQ_SUB );
  if(my_subscriber == NULL){
    printf("zmq error creating subscriber socket\n");
    return 1;
  }
  for( int n = 0; n < in_addy_str.size(); n++ ){
    try{
      my_subscriber->connect( in_addy_str[n].c_str() );
    }
    catch(std::exception& e){
      printf("zmq error connecting subscriber socket with in_addy_str: %s\n",
	     in_addy_str[n].c_str() );
      printf("Exception message: _%s_\n", e.what() );
      return 1;
    }
  }
  const char *filter = "";
  try{
    my_subscriber->setsockopt( ZMQ_SUBSCRIBE, filter, 0 );
  }
  catch(std::exception& e){
    printf("zmq error setting subscriber socket options.\n");
    printf("Exception message: _%s_\n", e.what() );
    return 1;
  }

  if( is_master == true ){
    my_master_secondary_listener = new zmq::socket_t( *my_zmq_context, ZMQ_REP );
    if( my_master_secondary_listener == NULL ){
      printf("zmq error in listen_in_thread creating REP socket\n");
      printf("secondary_in_addy_str is: _%s_\n", secondary_in_addy_str.c_str() );
      return 5;
    }
    try{
      my_master_secondary_listener->bind( secondary_in_addy_str.c_str() );
    }
    catch(std::exception& e){
      printf("Arte_command_port::listen_in_thread error binding port \n");
      printf("with string _%s_\n", secondary_in_addy_str.c_str() );
      printf("exception.what(): _%s_ \n", e.what() );
    }
    printf("SUCCESSFULLY SET UP REP SOCKET AND BOUND IT.\n"); //TODO: remove
    zmq::pollitem_t items [] = {
      { *my_master_secondary_listener, 0, ZMQ_POLLIN, 0 },
      { *my_subscriber,                0, ZMQ_POLLIN, 0 }
    };

    while (running){
      zmq::message_t z_msg;
      zmq::poll (&items[0], 2, -1);
      if( items[0].revents & ZMQ_POLLIN){
	try{   // TODO: put try blocks back in poll loop
	  my_master_secondary_listener->recv( &z_msg, 0 );
	}
	catch(std::exception& e){
	  printf("Arte_command_port master from-slave ");
	  printf("poll recv error.  What(): _%s_\n", e.what());
	}
	handle_message_from_slave( z_msg );
      }
      
      if(items[1].revents & ZMQ_POLLIN ){
	try{
	  my_subscriber->recv( &z_msg, 0 );
	}
	catch(std::exception& e){
	  printf("Arte_command_port primary recv error. what(): _%s_\n", e.what());
	  continue;
	}
	handle_primary_message( z_msg );
      }
    }
  } // end if for master case

  if(is_master == false){

    while (running){
    zmq::message_t z_msg;
    //    ArteCommand this_command_pb;

    //this_command_pb.Clear();
    try{
      my_subscriber->recv( &z_msg,0 );
    }
    catch(std::exception& e){
      printf("listen: Exception in Arte_command_port::listen_in_thread.\n");
      printf("listen: what(): _%s_ \n", e.what());
    }

    //std::string command_str( (char*) z_msg.data() );
    //command_str.resize( (int) z_msg.size(), 'a' );
    
    //this_command_pb.Clear();
    //if( !this_command_pb.ParseFromString( command_str )){
    //  printf("Arte_command_port parse error on string: _%s_\n",
    //	     command_str.c_str());
    //  continue;
    //}

    // If we've reached this point we have a well-formed protocol buffers message
    // Push it onto the queue and call the callback specified by the client
    //command_queue.push(this_command_pb);
    //callback_fn (callback_arg);
    //z_msg.rebuild();
    handle_primary_message(z_msg);

    }
  }

  return 0; // clean exit from the listening loop
}

int Arte_command_port::handle_primary_message(zmq::message_t& z_msg)
{
  ArteCommand this_command_pb;
  this_command_pb.Clear();
  std::string command_str( (char*) z_msg.data() );
  command_str.resize( (int) z_msg.size(), 'a' );

  this_command_pb.Clear();
  if( !this_command_pb.ParseFromString( command_str )){
    printf("Arte_command_port::handle_primary_message parse error ");
    printf("on string: _%s_\n", command_str.c_str() );
    return 1;  // exit error from primary message handler
  }

  // If we've reached this point, we have a well-formed protocol buffers message
  // Push it onto the queue and call the callback specified by the client
  command_queue.push(this_command_pb);
  callback_fn( callback_arg );
  z_msg.rebuild();

  return 0; // clean exit from primary message handler
}

int Arte_command_port::handle_message_from_slave(zmq::message_t& z_msg)
{

  zmq::message_t outgoing( z_msg.size() );
  zmq::message_t outgoing_response( z_msg.size() );
  memcpy( outgoing.data(), z_msg.data(), z_msg.size() );
  memcpy( outgoing_response.data(), z_msg.data(), z_msg.size() );
  try{
    my_master_secondary_listener->send( outgoing_response );
  }
  catch(std::exception &e){
    printf("Arte_command_port::handle_message_from_slave ");
    printf("Error replying to REQ. what(): _%s_\n", e.what() );
  }
  try{
    my_publisher->send( outgoing );
  }
  catch(std::exception& e){
    printf("Exception from Arte_command_port::handle_message_from_slave ");
    printf("\n what(): _%s_\n", e.what());
    return 2;
  }
  z_msg.rebuild();
  return 0;
}

void Arte_command_port::debug_print_strings()
{
  printf("Arte_command_port::debug_print_strings ***************\n");
    for(int n = 0; n < in_addy_str.size(); n++){
      printf("in_addy_str[%d] = _%s_\n", n, in_addy_str[n].c_str() );
    }
  printf("out_addy_str_m = _%s_\n", out_addy_str_m.c_str() );
  printf("out_addy_str_s = _%s_\n", out_addy_str_s.c_str() );
  printf("secondary_in_addy_str = _%s_\n", secondary_in_addy_str.c_str() );
  printf("******************************************************\n");
}

#include <stdio.h>
#include <stdint.h>
#include "arte_command_port.h"

//************* Simple constructor ****************/
Arte_command_port::Arte_command_port()
{
  basic_inin();
}


//**** Constructor with zmq-style addresses *******/
Arte_command_port::Arte_command_port(char *in_addy_char,
				     char *out_addy_char)
{
  basic_init();
  set_addy_str( in_addy_char, out_addy_char );
}


//*** Constructor with addresses and callback fn ****/
Arte_command_port::Arte_command_port(char *in_addy_char,
				     char *out_addy_char,
				     void CallbackFn(void *arg))
{
  basic_init();
  set_addy_str( in_addy_char, out_addy_char );
  set_callback_fn( CallbackFn, arg );
}


//***** Zero out fields that need to be set *********/
void Arte_command_port::basic_init()
{
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  running = false;
  in_addy_str.erase();
  out_addy_str.erase();
  callback_fn = NULL;
  callback_arg = NULL;
}


//****** Simple addr string setter *****************/
void Arte_command_port::set_addy_str( char *in_addy_char,
				      char *out_addy_char)
{
  addy_str.assign( in_addy_char, out_addy_char );
}


//********* Simple callback setter *****************/
void Arte_command_port::set_callback_fn( void CallbackFn( void *arg) )
{
  callback_fn = CallbackFn;
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

  zmq::context_t context(1);
  my_zmq_context = context;
  if(my_zmq_context == NULL){
    printf("zmq error creating context\n");
    return 1;
  }

  running = true;
  int rc = init_send();
  if(rc){
    printf("Arte_command_port failed to initialize publisher.\n");
    return 1;
  }

  int rc = pthread_create( listener_thread,       /* thread_t          */
			   NULL,                  /* thread attributes */
			   listen_in_thread_wrap, /* thread fn         */
			   this);                 /* arg to thread fn  */
  if(rc){
    running = false;
    printf("Arte_command_port failed to listen in thread.\n");
    return 1;
  }

}


//*********** Publish an ArteCommand to the network *************/
int Arte_command_port::send_command(ArteCommand& the_command)
{
  std::string command_str;
  if( !(the_command.SerializeToString( &command_str ))){
    printf("Arte_command::send_command failed to serialize the ArteCommand.\n");
    return 1;
  }
  zmq::message_t z_msg ( (void*) (command_str.c_str()), 
			 strlen(command_str.c_str()), NULL);
  if(z_msg == NULL){
    printf("Arte_command::send_command couldn't create a message from the buffer.\n");
    return 1;
  }
  
  int rc = my_publisher.send( z_msg );
  if(rc){
    printf("Arte_command::send_command couldn't send buffer over the socket.\n");
    return 1;
  }

  return 0;
}


Arte_command_port::~Arte_command_port()
{
  
  delete my_publisher; // this was newed in init_send();
                       // my subscriber, on the other hand, was
                       // a stack object

  //zmq_term(1)?       // the zmq c++ examples don't seem to do this.
  //                   // is it handled during destruction of the context
  //                   // when the context_t obejct goes out of scope?
  //                   // ...
  //                   // Just read the docs.  That is in fact how context works ^^
}


//******* Pop the command from the front of the queue ********/
ArteCommand Arte_command_port::command_queue_pop()
{
  ArteCommand ret_val;
  ret_val.CopyFrom( command_queue.front() );
  command_que.pop();
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
  return ( !(in_addy_str.empty() |
	     out_addy_str.empty() |
	     (callback_fn == NULL) | 
	     (callback_arg == NULL)) );
}


//************ Initialize the publisher socket ***************/
int Arte_command_port::init_send()
{
  my_publisher = new publisher( *my_context, ZMQ_PUB );
  if( my_publisher == NULL ){
    printf("Arte_command_port failed to create publisher socket.\n");
    return 1;
  }
  my_publisher->bind( out_addy_str.c_str() );
  
}


//**** Static wrapper function to use as fn ptr for thread call ****/
static int Arte_command_port::listen_in_thread_wrap(void *arg)
{
  // Cast arg to Arte_command_port and call listen_in_thread method
  ( (Arte_command_port*)arg )->listen_in_thread;
}


//******** Start thread and do listening loop in that thread ********/
int Arte_command_port::listen_in_thread()
{
  // Establish zmq subscriber connection. Listen to port and
  // parse messages to ArteCommands.
  zmq::socket_t subscriber ( *my_context, ZMQ_SUB );
  my_subscriber = &subscriber;
  if(subscriber == NULL){
    printf("zmq error creating subscriber socket\n");
    return 1;
  }
  int rc = subscriber.connect( in_addy_str.c_str() );
  if( rc ){
    printf("zmq error connecting subscriber socket with in_addy_str: %s\n",
	   in_addy_str.c_str() );
    return 1;
  }
  const char *filter = "";
  rc = subscriber.setsockopt( ZMQ_SUBSCRIBE, filter, NULL );
  if (rc ){
    printf("zmq error setting subscriber socket options.\n");
    return 1;
  }

  while (running){
    zmq::message_t z_msg;
    ArteCommand this_command_pb;
    this_command_pb.Clear();
    subscriber.recv( &z_msg );
    if( !this_command_pb.ParseFromString( static_cast<char*> ( z_msg.data() )) ){
      printf("subscriber failed to parse to received string:%s",
	     static_cast<char*> (z_msg.data()) );
      continue;
    }
    // If we've reached this point we have a well-formed protocol buffers message
    // Push it onto the queue and call the callback specified by the client
    command_queue.push(this_command_pb);
    callback_fn (callback_arg);
  }
  return 0; // clean exit from the listening loop
}


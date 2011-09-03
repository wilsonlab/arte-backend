#include <stdio.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "../arte_command.pb.h"
#include "../arte_command_port.h"
#include "test_arte_command_port.h"


// global variables
ArteCommand my_message_out;
ArteCommand my_message_in;
Arte_command_port* my_port;  


MsgSenderGetter::MsgSenderGetter( std::string &addy_str, CALLBACK_FN cb_f ){
  my_command_port.set_addy_str( addy_str, addy_str );
  //my_command_port.set_callback_fn( cb_f,  (void*) this );
  my_command_port.set_callback_fn( cb_f, (void*)this );
  my_command_port.start();
}

void MsgSenderGetter::send_msg( char *msg_str ){
    my_message.Clear();
    my_message.set_message_string( msg_str );
    my_command_port.send_command( my_message );
}

ArteCommand MsgSenderGetter::get_msg(){ 
  //ArteCommand my_message = my_command_port.command_queue_pop();
      ArteCommand my_message;
      my_message.CopyFrom( my_command_port.command_queue_pop() );
  return my_message;
}

void test_fn(void *arg){
  printf("Callback: Test.\n");
  //( (MsgSenderGetter*)arg )->read_and_print();
  //  sleep(1);
  for(int n = 0; n < my_port->command_queue_size(); n++){
    my_message_in.Clear();
        my_message_in = my_port->command_queue_pop();
    printf("Callback: Message #%d command string: _%s_\n",
    	   n, my_message_in.message_string().c_str());
    printf("Callback: ");
    my_message_in.PrintDebugString(); fflush(stdout);
    printf("\n");
  }
}

void MsgSenderGetter::read_and_print(){
  printf("test from MessageSenderGetter::read_and_print()\n"); fflush(stdout);
}

void MsgSenderGetter::print_msg(int *a)
{
  printf("in print_msg\n");
}

int main(int argc, char *argv[])
{

  boost::property_tree::ptree pt;
  
  if(argc != 2){
    printf("Usage: test_arte_command_port tcp://serverhostname:port_a");
    printf(" command_message   OR: \n");
    printf(" test_arte_command_port path_to/arte_setup_delfault.conf\n");
    return 1;
  }

  read_xml( std::string(argv[1]) , pt, boost::property_tree::xml_parser::trim_whitespace);
  
  my_port = new Arte_command_port( pt );
  my_port->set_callback_fn( &test_fn, (void *)(&my_message_out) );

  // std::string addy_str(argv[1]);
  //my_port = new Arte_command_port( addy_str, addy_str, &test_fn, (void *)(&my_message_out));

  my_port->start();

  //my_port->send_command( my_message_out );
  std::string user_in;
  //std::istringstream iss;

  printf("Enter string to publish.  Or 'exit' to stop listening.\n");
  bool running = true;
  while( running ){
    user_in.erase();
    my_message_out.Clear();
    getline( std::cin, user_in );
    //    printf("finished getting string.\n"); fflush(stdout);
    if( user_in.compare("exit") == 0 ){
      running = false;
      continue;
    }
    my_message_out.set_message_string( user_in );
    //std::cout << "user_in: _" << user_in << "_\n"; fflush(stdout);
    my_port->send_command( my_message_out );
  }
  printf("Going to stop the port.\n");
  my_port->stop();
  printf("Reached end of main.\n"); fflush(stdout);

  return 0;
}
  

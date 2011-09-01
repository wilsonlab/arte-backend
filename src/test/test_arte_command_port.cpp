#include <stdio.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include "../arte_command.pb.h"
#include "../arte_command_port.h"
#include "test_arte_command_port.h"

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
  printf("Test.\n");
}

void MsgSenderGetter::print_msg(int *a)
{
  printf("in print_msg\n");
}
  
int main(int argc, char *argv[])
{

  ArteCommand my_message_out;
  ArteCommand my_message_in;
  
  if(argc != 3){
    printf("Usage: test_arte_command_port tcp://serverhostname:port_a");
    printf(" command_message\n");
    return 1;
  }
  
  std::string addy_str(argv[1]);
  Arte_command_port my_port( addy_str, addy_str, &test_fn, (void *)(&my_message_out));
  
  if(argc > 1){
    my_message_out.set_message_string( argv[2] );
  }

  my_port.start();

  my_port.send_command( my_message_out );
  std::string user_in;
  //std::istringstream iss;

  printf("Enter string to publish.  Or 'exit' to stop listening.\n");
  bool running = true;
  while( running ){
    user_in.erase();
    my_message_out.Clear();
    getline( std::cin, user_in );
    printf("finished getting string.\n");
    if( user_in.compare("exit") == 0 ){
      running = false;
      continue;
    }
    my_message_out.set_message_string( user_in );
    std::cout << "user_in: _" << user_in << "_\n";
    my_port.send_command( my_message_out );
  }
  printf("Going to stop the port.\n");
  my_port.stop();
  printf("Reached end of main.\n"); fflush(stdout);

  return 0;
}
  

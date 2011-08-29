#include <stdio.h>
#include <stdint.h>
#include <zmq.h>
#include "arte_command_port.h"

Arte_command_port::Arte_command_port()
{
  basic_inin();
}

Arte_command_port::Arte_command_port(char *in_addy_char,
				     char *out_addy_char)
{
  basic_init();
  set_addy_str( in_addy_char, out_addy_char );
}

Arte_command_port::Arte_command_port(char *in_addy_char,
				     char *out_addy_char,
				     void CallbackFn(void *arg))
{
  basic_init();
  set_addy_str( in_addy_char, out_addy_char );
  set_callback_fn( CallbackFn, arg );
}

void Arte_command_port::basic_init()
{
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  running = false;
  in_addy_str.erase();
  out_addy_str.erase();
  callback_fn = NULL;
  callback_arg = NULL;
}

void Arte_command_port::set_addy_str( char *in_addy_char,
				      char *out_addy_char)
{
  addy_str.assign( in_addy_char, out_addy_char );
}

void Arte_command_port::set_callback_fn( void CallbackFn( void *arg) )
{
  callback_fn = CallbackFn;
  callback_arg = arg;
}

bool Arte_command_port::ok_to_start()
{
  return ( !(in_addy_str.empty() |
	     out_addy_str.empty() |
	     (callback_fn == NULL) | 
	     (callback_arg == NULL)) );
}

int Arte_command_port::start()
{
  if( !ok_to_start() ){
    printf("Arte_command_port couldn't start; uninitialized fields.\n");
    return 1;
  }

  running = true;
  int rc = pthread_create( listener_thread,       /* thread_t          */
			   NULL,                  /* thread attributes */
			   listen_in_thread_wrap, /* thread fn         */
			   this);                 /* arg to thread fn  */
  if(rc){
    running = false;
    printf("Arte_command_port failed to listen in thread.\n");
  }
}

static int Arte_command_port::listen_in_thread_wrap(void *arg)
{
  // Cast arg to Arte_command_port and call listen_in_thread method
  ( (Arte_command_port*)arg )->listen_in_thread;
}

int Arte_command_port::listen_in_thread()
{
  zmq::context_t context(1);
  my_zmq_context = context;
  if(my_zmq_context == NULL){
    printf("zmq error creating context\n");
    return 1;
  }
  zmq::socket_t subscriber ( context, ZMQ_SUB );
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
  
  

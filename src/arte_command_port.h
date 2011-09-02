// Arte_command_port class sends/receives user-packed ArteCommand objects
// through a zmq pub-sub port.  Clients should include ArteCommand.pb.h
// and know how to write and read ArteCommand google protocol buffers;
// The client owning this class should specify a callback function and
// parameter.  This function will be called from this class to notify the
// client every time a new ArteCommand comes in through zmq.
// Clients can then check the size of the stack and consume ArteCommands.
//
// greghale@mit.edu

#ifndef _ARTE_COMMAND_PORT_H
#define _ARTE_COMMAND_PORT_H

#include <pthread.h>
#include <zmq.hpp>
#include <queue>
#include <time.h>
#include <sys/time.h>
#include "arte_command.pb.h"

typedef void (*CALLBACK_FN)(void *);

class Arte_command_port{

 public:
  Arte_command_port();                               // Construct no init                   
  Arte_command_port(std::string& in_addy_char,
		    std::string& out_addy_char);            // Construct w/ ip:port

  Arte_command_port(std::string& in_addy_char,
		    std::string& out_addy_char,             // Construct w/ ip:port
		    CALLBACK_FN, 
		    void *arg);  //   and notifier fn
 
  ~Arte_command_port();

  void set_addy_str( std::string&, std::string& );
  //void set_callback_fn( int (*CallbackFn)(int), (void *arg) );
  void set_callback_fn( CALLBACK_FN, void* );

  int start();                                      // Start listener thread
  int stop();                                       // Stop listener thread

  int send_command(ArteCommand& the_command);        // Publish a command to network

  int command_queue_size();                          // How many commands waiting
  ArteCommand command_queue_pop();                  // Retrieve oldest command
                                                     // and delete it from queue

  void Testfire_callback();                          // Call the callback fn, if you want
                                                     // to make sure it's properly wired
                                                     // up.


  
 private:
  void basic_init();         //
  bool ok_to_start();        //
  int  init_send();

  int initialize_publisher();
  int listen_in_thread();

  static void* listen_in_thread_wrapper(void *arg);

  bool running;
  std::string in_addy_str;    
  std::string out_addy_str;
  void (*callback_fn)(void *);
  void *callback_arg;

  std::queue <ArteCommand> command_queue;

  zmq::context_t *my_zmq_context;
  zmq::socket_t  *my_subscriber;
  zmq::socket_t  *my_publisher;
  pthread_t listener_thread;

  timespec pause_dur;

};
#endif

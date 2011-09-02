// Arte_command_port class sends/receives user-packed ArteCommand objects
// through a zmq pub-sub port.  Clients should include ArteCommand.pb.h
// and know how to write and read ArteCommand google protocol buffers;
// The client owning this class should specify a callback function and
// parameter.  This function will be called from this class to notify the
// client every time a new ArteCommand comes in through zmq.
// Clients can then check the size of the stack and consume ArteCommands.
//
// For example usage, see src/test/test_arte_command_port.cpp main function
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
  Arte_command_port();                            // Construct no init
  Arte_command_port(std::string& in_addy_char,
		    std::string& out_addy_char);  // Construct w/ ip:port

  Arte_command_port(std::string& in_addy_char,
		    std::string& out_addy_char,   // Construct w/ ip:port
		    CALLBACK_FN, 
		    void *arg);                   //   and notifier fn
 
  ~Arte_command_port();

  void set_addy_str( std::string&, std::string& );

  void set_callback_fn( CALLBACK_FN, void* );

  int start();                          // Start listener thread & sending
  int stop();                           // Stop listener thread & sending

  int send_command(ArteCommand);        // Publish a command to network

  int command_queue_size();             // How many commands waiting
  ArteCommand command_queue_pop();      // Retrieve oldest command
                                        // and delete it from queue

  void Testfire_callback();             // Call the callback fn, if you want
                                        // to make sure it's properly wired
                                        // up.


  
 private:
  // zero and null most members
  void basic_init();         

  // check that needed data for starting are non-null
  bool ok_to_start();

  // bring up zmq sending socket
  int  init_send();

  // in a thread, bring up receiving socket and wait
  // for commands in a loop; put incoming commands
  // in command_queue
  int listen_in_thread();

  // static wrapper for thread - call signature conforms
  // to the one needed by pthread_create.  The wrapper
  // simply calls the class method listen_in_thread
  // (arg is 'this' from object calling pthread_create
  static void* listen_in_thread_wrapper(void *arg);

  // Flag used in listener thread to break out of
  // listening loop
  bool running;

  // For zmq::socket_t my_subscriber, ip:port
  // (usually matches out_addy_str - zmq does
  // both sending and receiving from a single
  // port)
  std::string in_addy_str;    
  
  // For zmq::socket_t my_publisher, ip:port 
  // (usually matches in_addy_str)
  std::string out_addy_str;

  // Function pointer for callback.  Callback
  // function must take void pointer and return
  // nothing
  void (*callback_fn)(void *);

  // Argument to pass to callback function.
  // Normally 'this' for object owning this 
  // arte_command_port, or any other object
  // that will handle ArteCommand s
  void *callback_arg;

  // first-in first-out list of command
  std::queue <ArteCommand> command_queue;

  // pointers to zmq objects
  zmq::context_t *my_zmq_context;
  zmq::socket_t  *my_subscriber;
  zmq::socket_t  *my_publisher;

  pthread_t listener_thread;

};
#endif

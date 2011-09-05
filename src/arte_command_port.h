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
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "arte_command.pb.h"

typedef void (*CALLBACK_FN)(void *);              // Our callback function returns
                                                  // nothing, takes void* argument

class Arte_command_port{

 public:

  // Three constructors for daily use - pass in property tree for configuration

  Arte_command_port();                           // Construct no init

  Arte_command_port(boost::property_tree::ptree, // Construct from setup conf
		    CALLBACK_FN, 
		    void* arg,
		    bool auto_start);

  Arte_command_port(std::string pt_pathname,     // Construct from conf filename
		    CALLBACK_FN, 
		    void* arg, 
		    bool auto_start);
 
  ~Arte_command_port();

  void set_addy_str( std::vector<std::string>, // vector <zmq-style in addy string>
		                               // (Master=SUB, Slave=REQ)
		     std::string&,             // zmq-style out addy string (PUB)
		     std::string&,             // zmq-stly out slave string (REQ)
		     std::string &);           // zmq-style master 2nd in addy (REP)

  void reset_addy_str();

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

  void do_pt_settings();
  
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

  int handle_primary_message(zmq::message_t&);

  int handle_message_from_slave(zmq::message_t&);

  // Flag used in listener thread to break out of
  // listening loop
  bool running;

  // For zmq::socket_t my_subscriber, ip:port
  // (usually matches out_addy_str - zmq does
  // both sending and receiving from a single
  // port)
  // There is one of these for each host on
  // the network (commands are shared fan-in
  // style.
  std::vector <std::string> in_addy_str;    

  // Master listens as REP server to this addy too
  std::string secondary_in_addy_str;

  // For zmq::socket_t my_publisher, ip:port 
  // (usually matches in_addy_str)
  std::string out_addy_str_m;
  std::string out_addy_str_s;

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
  zmq::socket_t  *my_master_secondary_listener;

  pthread_t listener_thread;

  // one master command process per machine
  // this is because zmq won't allow 2 
  // servers to bind the same PUB socket
  // for sending.  The workaround is, we
  // Try to bind this socket.  If we get it,
  // We are the master.  Masters both subscribe
  // to and publish on the main command port.
  // Masters are also the server in a REP-REQ
  // topology, on the SECONDARY port.  Any
  // messages coming to the master from this
  // port must be forwarded to the primary
  // port by the master process.
  // A non-master process is a slave.  Slaves
  // are clients (doing a connect, not bind)
  // in REQ_REP topology, and then sending
  // their REQ messages to the secondary port
  bool is_master;
  std::string primary_port;
  std::string secondary_port;

  boost::property_tree::ptree *my_pt;
  std::string pt_fn;

  bool _auto_start;

  void debug_print_strings();

};
#endif

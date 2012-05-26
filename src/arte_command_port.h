//************************ How to use the class ****************************/
//*
//*  Arte_command_port - a node in a network of communicating processes
//*
//*  An Arte_command_port instance is owned by each running processes
//*  that wants to receive ArteCommand packets.  These packets are
//*  instances of a google protobuf serialization object ( see
//*  http://code.google.com/p/protobuf/ for info about protobufs).
//*
//*  ArteCommands are described in arte_command.proto; which is
//*  compiled by protoc to arte_command.pb.cpp and arte_command.pb.h
//*  
//*  ArteCommands are heirarchical.  Each has a toplevel with descriptive
//*  fields relevant to any command (originator name, intended source name,
//*  timestamp, etc), as well as a long list of optional sub-commands
//*
//*  Clients that want to send and receive commands follow a few
//*  fairly simple steps.
//*
//*  First, create a function to use as a callback function pointer.
//*  Arte_command_port will call this function when there are commands
//*  waiting in the inbox
//*
//*  Second, instantiate Arte_command_port, pass it the arte_setup property
//*  tree, and start it.  The instance will decide whether it should run
//*  as master or slave (explained soon).
//*
//*  When client wants to SEND a message, instantiate an ArteCommand,
//*  set the fields of the command with the data you want to pass,
//*  and call the instance's send_command(ArteCommand)  method
//*
//*  When you want to process a command, call the instances's 
//*  command_port_pop() method.
//*
//*  The number of commands waiting to be processed can be checked with
//*  the instance's command_queue_size() method.
//*
//*  The easiest way to respond commands is simply to call
//*  command_queue_pop() every time Arte_command_port calls your
//*  designated callback function.  Then command_queue_size()
//*  to make sure that all waiting commands have been processed.
//*
//*  The inboxes between different processes are independent.  One
//*  instance of Arte_command_port has a single inbox.  If multiple
//*  clients share a single Arte_command_port, they run the risk
//*  of disposing of each other's commands before the other has a chance
//*  to read.  Therefore a single process should have only one
//*  Arte_command_port  instance.
//*
//*  Multiple processes on the same machine may have independent
//*  Arte_command_port instances.  Messages are broadcast to every
//*  process on every machine in the network described in the arte
//*  setup property tree.
//*
//*  Arte_command_port instances decide among themselves wich will be
//*  master.  On a given machine, there is exactly one master instance.
//*  All other instances are slaves.  This is an implementation wart
//*  made necessary by a limitation of the socket protocol we use.
//*
//*  The master on a given machine is a hub that sends messages to itself,
//*  to every other machine on the network, and to all slave processes. 
//*  If the master process dies, no commands will be able to reach from 
//*  the outside network to the slaves, and no commands will be able to 
//*  move between slaves on a machine, or from those slaves to the rest 
//*  of the network.
//*
//*  There is no way to detect the occurance of the death of a master
//*  on one machine from another machine; but processes on the same
//*  machine will realize the death when they try to send a message,
//*  because they expect confirmation from the master, and this confirmation
//*  only comes when the master is running.
//*
//*  After a 1-second timeout, a slave waiting for confirmation will
//*  try to take ownership of the master role.  If it is the first do to
//*  so from among all slave processes, it will be master.  If it attemps
//*  to become master but another slave has already been designated new
//*  master, this late instance will resume its role as slave.
//*
//*  Because slaves negotiate master role upon death of the master, it is
//*  recommended that all clients send out periodic dummy messages; 
//*  this ensures that the pool of slaves on a machine with a dead master
//*  will not go for too long missing all incoming messages from the outside.
//* 
//*  Example usage in arte-ephys/src/test/test_arte_command_port.cpp
//*

#ifndef _ARTE_COMMAND_PORT_H
#define _ARTE_COMMAND_PORT_H

#include <pthread.h>
#include <zmq.hpp>
#include <queue>
#include <time.h>
#include <sys/time.h>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "arte_pb.pb.h"
#include "arte_command.pb.h"

typedef void (*CALLBACK_FN)(void *);              // Our callback function returns
                                                  // nothing, takes void* argument

class Arte_command_port{

 public:

  // Three constructors for daily use - pass in property tree for configuration

  Arte_command_port(); // Construct no init (unusual)

  // Construct from ArteSetupPb
  Arte_command_port( ArteSetupOptPb &setup_pb, CALLBACK_FN, void* arg, bool auto_start);

  Arte_command_port(boost::property_tree::ptree, // Construct from setup conf
		    CALLBACK_FN, void* arg, bool auto_start);

  Arte_command_port(std::string pt_pathname,     // Construct from conf filename
		    CALLBACK_FN, void* arg, bool auto_start);
 
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

  void  set_zmq_context_ptr(void*);
  void* get_zmq_context_ptr();

  void Testfire_callback();             // Call the callback fn, if you want
                                        // to make sure it's properly wired
                                        // up.


  
 private:
  // zero and null most members
  void basic_init();         

  void do_pt_settings();
  void do_pb_settings( ArteSetupOptPb &setup_pb );
  
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

  // Delete zmq objects that got newed
  void clear_memory();

  pthread_t listener_thread;

  bool is_master;
  std::string primary_port;
  std::string secondary_port;

  // set this to true if client passed in a
  // zmq::context_t.  In this case, don't mess
  // with the context when we close; presumably
  // the client will do this
  bool _external_context;

  boost::property_tree::ptree *my_pt;
  std::string pt_fn;

  bool _auto_start;

  void debug_print_strings();

};
#endif

#ifndef _TEST_ARTE_COMMAND_H
#define _TEST_ARTE_COMMAND_H

#include <string>
#include <time.h>
#include <sys/time.h>
#include "../arte_command.pb.h"
#include "./arte_command_port.h"

typedef void (*CALLBACK_FN)(void *);

class MsgSenderGetter{
 public:
  MsgSenderGetter();
  MsgSenderGetter( std::string& , CALLBACK_FN );
  ~MsgSenderGetter();
  void print_msg(int *a);
  void send_msg( char *msg_str );
  ArteCommand get_msg();
  void read_and_print();
 private:
  Arte_command_port my_command_port;
  ArteCommand my_message;
  void comget_message();
  timespec pause_dur;
};


#endif

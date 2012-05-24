#ifndef NETCOM_H
#define NETCOM_H

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "datapacket.h"
#include "arte_pb.pb.h"
#include "arte_command.pb.h"
#include "unp.h"

struct NetComDat{
	int sockfd;
	struct sockaddr_in addr_in;
	struct sockaddr_storage their_addr;
};

class NetCom{
 public:

  NetCom();

  static NetComDat initUdpTx(char host[], int port);
  static NetComDat initUdpRx(char host[], char * port);

  static int txTs(NetComDat net, timestamp_t count, int nTx);
  static timestamp_t rxTs(NetComDat net);

  static int txSpike(NetComDat net, spike_net_t* spike);
  static void rxSpike(NetComDat net, spike_net_t* spike);

  static int txWave(NetComDat net, lfp_bank_net_t *lfp);
  static void rxWave(NetComDat net, lfp_bank_net_t *lfp);

  static int txCommand(NetComDat net, command_t* the_command);
  static void rxCommand(NetComDat net, command_t* the_command);

  static void txBuff(NetComDat net, char * buff, int buff_len);
  static void rxBuff(NetComDat net, char * buff, int *buff_len);

  void txArtePb(NetComDat net, ArtePb& _arte_pb_to_write_from);
  void rxArtePb(NetComDat net, ArtePb& _arte_pb_to_write_to);

  char        tmp_buffer_char[BUFFSIZE];
  std::string tmp_buffer_string;

};

void *get_in_addr(struct sockaddr *sa);
/*
//static void char const base_cmd_ack_port[] = "15227";
//static void int  const n_cmd_ack_ports = 100;

#define portIntToString(x) boost::lexical_cast<string>(x)
*/
#endif


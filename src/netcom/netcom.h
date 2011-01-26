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
#include <byteswap.h>

#define ntoh64(x) bswap_64(x);
#define hton64(x) bswap_64(x);

#define ntoh32(x) bswap_32(x);
#define hton32(x) bswap_32(x);

#define ntoh16(x) bswap_16(x);
#define hton16(x) bswap_16(x);


//#define NETCOM_UDP_SPIKE 1
//#define NETCOM_UDP_LFP 2
//#define NETCOM_UDP_SYNC 3
#define MAXBUFLEN 2048

enum packetType_t {NETCOM_UDP_SPIKE = 1, 
		   NETCOM_UDP_LFP = 2, 
		   NETCOM_UDP_SYNC = 3, 
		   NETCOM_UNDEFINED=-1};

inline char typeToChar(packetType_t x){
	switch (x){
		case NETCOM_UDP_SPIKE: 	return 1;
		case NETCOM_UDP_LFP:	return 2;
		case NETCOM_UDP_SYNC:	return 3;
		default:		return -1;
	}
}

inline packetType_t charToType(char x){
	switch (x){
		case 1: return NETCOM_UDP_SPIKE;
		case 2:	return NETCOM_UDP_LFP;
		case 3: return NETCOM_UDP_SYNC;
		default:return NETCOM_UNDEFINED;
	}
}

struct NetComDat{
	int sockfd;
	struct sockaddr_in addr_in;
	struct sockaddr_storage their_addr;
};

class NetCom{
  public:
	static NetComDat initUdpTx(char host[], int port);
	static NetComDat initUdpRx(char host[], int port);
	static int txSyncCount(NetComDat net, uint32_t count, int nTx);
	static uint32_t rxSyncCount(NetComDat net);
//  private:
	static packetType_t getPacketType(char *buff);
	static void syncCountToBuffer(uint32_t count, char *buff, unsigned int blen);
	static uint32_t bufferToSyncCount(char *buff, unsigned int blen);
};

void *get_in_addr(struct sockaddr *sa);


#endif


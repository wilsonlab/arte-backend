#if defined(__linux__)
	#include <byteswap.h>
#else
	#ifndef _BYTESWAP_H
	#define _BYTESWAP_H
		static inline unsigned short bswap_16(unsigned short x) {
			return (x>>8) | (x<<8);
		}

		static inline unsigned int bswap_32(unsigned int x) {
			return (bswap_16(x&0xffff)<<16) | (bswap_16(x>>16));
		}

		static inline unsigned long long bswap_64(unsigned long long x) {
			return (((unsigned long long)bswap_32(x&0xffffffffull))<<32) |
			(bswap_32(x>>32));
		}
	#endif
#endif

#include <vector>

#ifndef NETCOM_DATAPACKET_H
#define NETCOM_DATAPACKET_H

#include <stdint.h>
#include <netinet/in.h>
#include "global_defs.h"

// these seem to assume that host byteorder is little endian
// should they be replaced with ntohs(x), ntohl(x), etc?
// and, how would we do this with the 64 bit values?
#define ntoh64(x) bswap_64(x)
#define hton64(x) bswap_64(x)

#define ntoh32(x) bswap_32(x)
#define hton32(x) bswap_32(x)

#define ntoh16(x) bswap_16(x)
#define hton16(x) bswap_16(x)


// Conditional versions of the above, so that datapacket
// functions can also build bytestreams for saving to disk
// (in which case, we don't want any bswaps to happen)
#define ntoh64c(x,c)  c ? bswap_64(x) : x
#define hton64c(x,c)  c ? bswap_64(x) : x

#define ntoh32c(x,c)  c ? bswap_32(x) : x
#define hton32c(x,c)  c ? bswap_32(x) : x

#define ntoh16c(x,c)  c ? bswap_16(x) : x
#define hton16c(x,c)  c ? bswap_16(x) : x

#define MAX_BUF_LEN 2048
void printBuff(char* buff, int blen);

typedef uint32_t timestamp_t;

struct spike_net_t{
  timestamp_t ts;                                           // bytes 2:5
  uint16_t    name;                                         // bytes 6:7
  uint16_t    n_chans;                                      // bytes 8:9
  uint16_t    n_samps_per_chan;                             // bytes 10:11
  uint16_t    samp_n_bytes;                                 // bytes 11:12
  rdata_t     data[MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT]; // bytes 13: 13+( c * s * b )
  int16_t     gains[MAX_FILTERED_BUFFER_N_CHANS];           // the next 2*c bytes
  rdata_t     thresh[MAX_FILTERED_BUFFER_N_CHANS];          // the next b*c bytes
  uint16_t    trig_ind;                                     // the next 2 bytes
  uint32_t    seq_num;
};

struct lfp_bank_net_t{
  timestamp_t ts;                 // bytes 2:5
  uint16_t    name;               // bytes 6:7  does this field exist for lfp_banks?
  uint16_t    n_chans;            // bytes 8:9
  uint16_t    n_samps_per_chan;   // bytes 10:11
  uint16_t    samp_n_bytes;       // bytes 11:12
  rdata_t     data[MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT]; // bytes 13 : 13*s*c*b
  int16_t     gains[MAX_FILTERED_BUFFER_N_CHANS];              // the next 2*c bytes
  uint32_t    seq_num;
};

struct command_t{
  uint16_t n_char;
  char command_str[MAX_COMMAND_STR_LEN];
  uint32_t nonce;
};

struct command_ack_t{
  uint32_t nonce;
};

void tsToBuff(timestamp_t* s,char* buff, int blen, bool c);
timestamp_t buffToTs(char* buff, int blen, bool c);

void spikeToBuff(spike_net_t* s, char* buff, int *blen, bool c);
void buffToSpike(spike_net_t *s, char *buff, bool c);

void waveToBuff(lfp_bank_net_t* lfp, char* buff, int *blen, bool c);
void buffToWave(lfp_bank_net_t *lfp, char *buff, bool c);

void commandToBuff(command_t *command, char* buff, int *blen, bool c);
void buffToCommand(command_t *command, char* buff, bool c);

void commandAckToBuff(command_t *command, char* buff, int *blen, bool c);
void buffAckToCommand(command_t *command, char* buff, bool c);

enum packetType_t {NETCOM_UDP_SPIKE = 65,
                   NETCOM_UDP_LFP = 66,
                   NETCOM_UDP_TIME = 67,
				   NETCOM_UDP_COMMAND = 68,
				   NETCOM_UDP_COMMAND_ACK = 69,
                   NETCOM_UNDEFINED=-1};

inline char typeToChar(packetType_t x){
  switch (x){
  case NETCOM_UDP_SPIKE:   return 65;
  case NETCOM_UDP_LFP:     return 66;
  case NETCOM_UDP_TIME:    return 67;
  case NETCOM_UDP_COMMAND: return 68;
  case NETCOM_UDP_COMMAND_ACK: return 69;
  default:                 return -1;
  }
}

inline packetType_t charToType(char x){
  switch (x){
  case 65: return NETCOM_UDP_SPIKE;
  case 66: return NETCOM_UDP_LFP;
  case 67: return NETCOM_UDP_TIME;
  case 68: return NETCOM_UDP_COMMAND;
  case 69: return NETCOM_UDP_COMMAND_ACK;
  default: return NETCOM_UNDEFINED;
  }
}


#endif

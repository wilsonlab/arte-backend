#ifndef NETCOM_DATAPACKET_H
#define NETCOM_DATAPACKET_H

#include <stdint.h>
#include <netcom.h>

// Each buffer has a 2 byte header, with the first byte corresponding to 
// the type of data contained in the buffer, the second byte is set to 0 
// but this isn't currently required as maybe in the future we will see 
// a need to add more information to the buffer header
//
// ??should we explicity test to see if the 2nd byte is set to zero as a 
// sanity test on the buffer? Not sure we should discuss this
//
// Unless otherwise specified byts in each packet are arranged 
// such that the first var in the type def corresponds to 
// the first set of bytes in the buffer, the next var is found in the 
// directly after the bytes allocated for the previous var and so on.
// Each var uses exactly the number of bytes in the buffer that uses in 
// the struct, for Example:
//
// if I had  struct data_t{
// uint8_t v1;
// uint8_t v2;
// uint16_t v3;
// };
//
// the data in the packed buffer would look like:
// [BYTE0 |  BYTE1 | BYTE2 | BYTE3 | BYTE4 | BYTE5 ]
// [DTYPE |    0   | VAR1  |  VAR2 |  VAR3 | VAR3  ]
//
typedef uint32_t timestamp_t;

struct spike_t{
	timestamp_t ts;
	uint8_t src;
	uint8_t filt;
	uint8_t nChan;
	uint8_t nSamp;
	uint16_t thold[];
	uint16_t data[];
};

struct wave_t{
	timestamp_t ts;
	uint8_t src;
	uint8_t filt;
	uint16_t nSamp;
	uint16_t data[];
};

// The xxToBuff functions add the appropriate buffer headers, the user 
// does not have to worry about adding the headers by hand	

void tsToBuff(timestamp_t* s,char* buff, int blen);
timestamp_t buffToTs(char* buff, int blen);

void spikeToBuff(spike_t* s, char* buff, int blen);
spike_t buffToSpike(char *buff, int blen);

void waveToBuff(wave_t* w, char* buff, int blen);
wave_t buffToWave(char* buff, int blen);


#endif

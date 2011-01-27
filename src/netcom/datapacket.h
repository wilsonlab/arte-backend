#ifndef NETCOM_SYNCPACKET_H
#define NETCOM_SYNCPACKET_H

#include <stdint.h>
#include <netcom.h>

typedef uint32_t timestamp_t;

struct timepacket_t{
	timestamp_t ts;
};

struct spikepacket_t{
	timestamp_t ts;
	uint8_t src;
	uint8_t filt;
	uint8_t nChan;
	uint8_t nSamp;
	uint16_t data[];
};

struct wavepacket_t{
	timestamp_t ts;
	uint8_t src;
	uint8_t filt;
	uint16_t nSamp;
	uint16_t data[];
};
	

void tsToBuff(timestamp_t * s,char  *buff, int blen);
timestamp_t buffToTs(char *buff, int blen);

void spikeToBuff(spikepacket_t *s, char *buff, int blen);
spikepacket_t buffToSpike(char *buff, int blen);

void waveToBuff(wavepacket_t *w, char *buff, int blen);
wavepacket_t buffToWave(char *buff, int blen);


#endif

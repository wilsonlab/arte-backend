#include <iostream>
#include "datapacket.h"


/*------------- TIME ------------*/
void tsToBuff(timestamp_t* t, char *buff, int blen){
	if (blen<6)
		std::cout<<"ERROR: Buffer is too short"<<std::endl;
			
	buff[0] = typeToChar(NETCOM_UDP_SYNC);
	buff[1] = 0;
	
	timestamp_t ts = hton32(*t);

	memcpy(buff+2, &ts, 4);
}

timestamp_t buffToTs(char *buff, int blen){

	if (blen<6)
		std::cout<<"Error buffer is too short"<<std::endl;

	timestamp_t s;
	memcpy(&s, buff+2, 4);	
	s = ntoh32(s);

	return s;
}
/*------------- SPIKE ------------*/
void spikeToBuff(spike_t* s, char* buff, int blen){
	std::cout<<"spikeToBuff(), isn't checking the buffer size, implement this!"<<std::endl;

	buff[0] = typeToChar(NETCOM_UDP_SPIKE); 
	buff[1] = 0; 

	uint32_t tmp32 = hton32(s->ts);
	uint16_t tmp16 = 0;
	memcpy(buff+2, &tmp32,4);
	memcpy(buff+6, &(s->src), 1);
	memcpy(buff+7, &(s->filt), 1);
	memcpy(buff+8, &(s->nChan),1);
	memcpy(buff+9, &(s->nSamp),1);

	int i, ind=10;

	for (i=0; i < s->nChan; i++){
		tmp16 = hton16(s->gain[i]);
		memcpy(buff+ind, &tmp16, 2);
		ind += 2;
	}

	for (i=0; i < s->nChan; i++){
		tmp16 = hton16(s->thold[i]);
		memcpy(buff+ind, &tmp16, 2);
		ind += 2;
	}

	for (i=0; i< (s->nChan)*(s->nSamp); i++){
		tmp16 = hton16(s->data[i]);
		memcpy(buff+ind, &tmp16, 2);
		ind += 2;
	}
}

spike_t buffToSpike( char* buff, int blen){
//TODO IMPLEMENT
	spike_t s;
	
	uint32_t tmp32;
	uint16_t tmp16;

	memcpy(&tmp32, buff+2, 4);
	s.ts = ntoh32(tmp32);

	memcpy(&(s.src), buff+6,1);
	memcpy(&(s.filt),buff+7, 1);
	memcpy(&(s.nChan), buff+8, 1);
	memcpy(&(s.nSamp), buff+9, 1); 
	
	int i, ind=10;

	s.gain.resize(s.nChan);
	for (i=0; i<s.nChan; i++){
		memcpy(&tmp16, buff+ind, 2);
		s.gain[i] = ntoh16(tmp16);
		ind += 2;
	}
	
	s.thold.resize(s.nChan);
	for (i=0; i<s.nChan; i++){
		memcpy(&tmp16, buff+ind, 2);
		s.thold[i] = ntoh16(tmp16);
		ind += 2;
	}

	s.data.resize(s.nChan*s.nSamp);
	for (i=0; i<(s.nChan * s.nSamp); i++){
		memcpy(&tmp16, buff+ind,2);
		uint16_t tmp2 = ntoh16(tmp16);
		s.data[i] = ntoh16(tmp16);
		ind += 2;
	}
	return s;
}

/*------------- WAVE ------------*/
void waveToBuff(wave_t* w, char* buff, int blen){
//TODO IMPLEMENT

}

wave_t buffToWave(char* buff, int blen){
//TODO IMPLEMENT

	wave_t w;
	return w;
}
 

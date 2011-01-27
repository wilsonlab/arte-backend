#include <iostream>
#include "datapacket.h"
#include <iostream>


/*------------- TIME ------------*/
void tsToBuff(timestamp_t* t, char *buff, int blen){
	
	buff[0] = typeToChar(NETCOM_UDP_SYNC);
	buff[1] = 0;
	
	uint32_t ts = hton32(*t);

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
//TODO IMPLEMENT
}

spike_t buffToSpike(char* buff, int blen){
//TODO IMPLEMENT

	spike_t s;
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
 

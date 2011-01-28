#include <iostream>
#include "datapacket.h"
#include <iostream>


/*------------- TIME ------------*/
void tsToBuff(timestamp_t* t, char *buff, int blen){
	if (blen<6)
		std::cout<<"ERROR: Buffer is too short"<<std::endl;
			
	buff[0] = typeToChar(NETCOM_UDP_SYNC);
	buff[1] = 0;
	
	timetsamp_t ts = hton32(*t);

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

	int i, ind;
	buff[0] = typeToChar(NETCOM_UDP_SPIKE); 
	buff[1] = 0; 

	memcpy(buff+2, &(hton32(s->ts)),4);
	memcpy(buff+6, &(s->src), 1);
	memcpy(buff+7, &(s->filt), 1);
	memcpy(buff+8, &(s->nChan),1);
	memcpy(buff+9, &(s->nSamp),1);
	ind = 10;

	for (i=0; i < s->nChan, i++)
		memcpy(buff+ind+i*2, &(hton16(s->thold[i])), 2);
	ind = ind+(i*2);

	for (i=0; i < s->nChan, i++)
		memcpy(buff+ind+(i*2), &(hton16(s->gains[i])), 2);
	ind = ind+(i*2);

	for (i=0; i< (s->nChan)*(s->nSamp), i++)
		memcpy(buff+ind+(i*2), &(hton16(s->data[i])), 2);
	
	// copy thresholds?
	// copy gains?
	// copy data vector

	
	

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
 

#include <iostream>
#include "datapacket.h"
#include <iostream>



//Global Buffer Layout
//buff[0] = packet type
//buff[1] = 0 leave open for use in future systems



//Sync Buffer Layout:
//Global Header
//buff[2-6] uint32_t timestamp count
void tsToBuff(timestamp_t * t, char *buff, int blen){
	
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

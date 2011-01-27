#include <datapacket.h>
#include <iostream>
#include <stdint.h>

int main(){

	timestamp_t ts = 100;

	char buff[6] = "\0";

	tsToBuff(&ts, buff, 6);
	
	timestamp_t tt;
	tt = buffToTs(buff, 6);
	std::cout<<ts<<" "<<tt<<std::endl;	
}

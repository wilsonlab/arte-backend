#include "netcom.h"
#include <iostream>
#include <stdint.h>

int main()
{
	
	uint32_t count = 12345;
	
	char *buff = new char[6];// "\0";

	std::cout<<"size of buff"<< sizeof *buff<<std::endl;
	NetCom::syncCountToBuffer(count, buff, 6);
	uint32_t count2; 
	count2 = NetCom::bufferToSyncCount(buff, 6);

	std::cout<<count<<" "<<count2<<std::endl;
}

#include "netcom.h"

int main(){

	char host[] = "10.121.43.255";
	std::cout<<"Listening for packets on: "<<host<<std::endl;	
	NetComDat net = NetCom::initUdpRx(host, 4000);
	NetCom::rxSyncCount(net);

	return 0;
}

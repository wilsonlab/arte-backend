#include "netcom.h"

int main(){

	std::cout<<"NetComUdpRx Text"<<std::endl;	
//	NetComDat net = NetCom::initUdpRx("10.121.43.255", 4000);
	NetComDat net = NetCom::initUdpRx("localhost", 4000);
	NetCom::rxSyncCount(net);

	return 0;
}

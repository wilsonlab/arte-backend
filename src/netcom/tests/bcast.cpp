#include "netcom.h"

int main(){
	
//	char host[] = "10.121.43.155";
	char host[] = "10.121.43.255";
//	char host[] = "localhost";
	std::cout<<"Broadcasting to:"<<host<<std::endl;

	NetComDat net = NetCom::initUdpTx(host,4000);
	NetCom::txTs(net, 1000, 1);

	return 0;
}

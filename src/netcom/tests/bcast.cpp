#include "netcom.h"

int main(){
	std::cout<<std::endl;	
//	char host[] = "10.121.43.155";
	char host[] = "10.121.43.255";
//	char host[] = "localhost";
	std::cout<<"Broadcasting to:"<<host<<std::endl;

	NetComDat net = NetCom::initUdpTx(host,4000);
	NetCom::txTs(net, 12345, 1);
//	NetCom::txTs(net, 2189640, 1);

	return 0;
}

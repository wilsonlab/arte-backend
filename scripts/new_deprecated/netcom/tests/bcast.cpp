#include "netcom.h"

int main(){
	timestamp_t ts = 123456789;
	std::cout<<std::endl;	
	char host[] = "10.121.43.255";
	std::cout<<"Broadcasting:"<<ts<<" to:"<<host<<std::endl;

	NetComDat net = NetCom::initUdpTx(host,4000);
	NetCom::txTs(net, ts, 1);

	return 0;
}

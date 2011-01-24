#include "netcom.h"

int main(){
	
//	char host[] = "10.121.43.155";
	char host[] = "10.121.43.255";
//	char host[] = "localhost";
	std::cout<<"Broadcasting to:"<<host<<std::endl;
	for (int i=0; i<10; i++)
	{
		NetComDat net = NetCom::initUdpTx(host,4000);
		NetCom::txSyncCount(net, i*1000, 1);
	}

	return 0;
}

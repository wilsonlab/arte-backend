#include "netcom.h"

int main(){
	
	for (int i=0; i<10; i++)
	{

		NetComDat net = NetCom::initUdpTx("localhost", 4000);
//		NetComDat net = NetCom::initUdpTx("10.121.43.255",4000);
		NetCom::txSyncCount(net, i*1000, i);
	}

	return 0;
}

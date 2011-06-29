#include "netcom.h"
#include "datapacket.h"
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

#define noise 100
spike_net_t genFakeSpike();

int main(){

	int spikePerSecond = 3;
	int sleep =  (1e6)/spikePerSecond;
	char host[] = "10.121.43.56";
	int port = 5000;
	NetComDat net = NetCom::initUdpTx(host,port);

	spike_net_t spike = genFakeSpike();
	while (true)
	{
		NetCom::txSpike(net, &spike);
		usleep(sleep);
		spike = genFakeSpike();

//		std::cout<<"Sending spike"<<std::endl;
//		std::flush();
	}


}
spike_net_t genFakeSpike(){

	uint16_t trace[32] =
//        {11, 11, 11, 11, 11, 11, 11, 15, 20, 50, 100, 500, 100, 0,
//        0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10 };

	{100, 	100, 	100, 	100, 	100,	100,	110, 	120,
	 150, 	200, 	1000,	1800,	1000, 	200,	0,	5,
	 10,	20,	30,	40,	50,	60,	70,	80,
	 90,	91,	93,	94,	95,	96,	97,	98};

        spike_net_t s;
        s.ts = 987654321;
        s.name = 75;
        s.n_chans = 4;
        s.n_samps_per_chan = 32;
        s.samp_n_bytes = 2;

        int idx;
	srand( time(NULL));
        for (int i=0; i<s.n_chans; i++)
        {
                s.gains[i] = 1000;
                s.thresh[i] = 100;

                for (int j=0; j<s.n_samps_per_chan; j++){
                        idx = (i*s.n_samps_per_chan)+j;
                        s.data[idx] =  trace[j]/2 + rand() % noise;
                }
        }
        return s;
}

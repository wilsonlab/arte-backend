#include "netcom.h"
#include "datapacket.h"
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

spike_net_t genFakeSpike();
timeval then, now;
int nSpike =0;
int noise = 400;

int main(int argc, char* argv[]){

	
	if (argc<4)
	{
		std::cout<<"Usage: txArteSpikes ip port rate"<<std::endl;
		return 0;
	}
	char *host = argv[1];
	int port = atoi(argv[2]);
	int spikePerSecond =  atoi(argv[3]);

	if (argc>4)
		noise = atoi(argv[4]);
	else
		noise = 0;

	srand( time(NULL));

//	int spikePerSecond = 50;
	int sleep =  (1e6)/spikePerSecond;
//	char host[] = "10.121.43.56";
//	int port = 6303;
	NetComDat net = NetCom::initUdpTx(host,port);

	spike_net_t spike = genFakeSpike();
	gettimeofday(&then,NULL);
	while (true)
	{
		NetCom::txSpike(net, &spike);
		usleep(sleep);
		spike = genFakeSpike();
		gettimeofday(&now,NULL);
		long dt = (now.tv_usec - then.tv_usec);
		then.tv_usec = now.tv_usec;
//		std::cout<<"Transmitting spikes at rate:"<<1e6/dt<<std::endl;		
//		std::cout<<"Sending spike"<<std::endl;
//		std::flush();
	}


}
spike_net_t genFakeSpike(){

	uint16_t trace[32] =
//        {11, 11, 11, 11, 11, 11, 11, 15, 20, 50, 100, 500, 100, 0,
//        0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 10, 10, 10 };

/*	{100, 	100, 	100, 	100, 	100,	100,	110, 	120,
	 150, 	200, 	1000,	1800,	1000, 	200,	1,	5,
	 10,	20,	30,	40,	50,	60,	70,	80,
	 90,	91,	93,	94,	95,	96,	97,	98};*/
	{	65, 	78, 	90, 	101,	111,	120,	126,	130,
		129,	126,	120,	111,	101,	90,		78,		65,
		52,		40,		29,		19,		11,		5,		2,		1,
		2,		5,		11,		19,		29,		40,		52,		60};


        spike_net_t s;
        s.ts = 987654321;
        s.name = 75;
        s.n_chans = 4;
        s.n_samps_per_chan = 32;
        s.samp_n_bytes = 2;

        int idx=0;
		nSpike++;
        for (int i=0; i<s.n_samps_per_chan; i++)
        {
                s.gains[i] = 1000;
                s.thresh[i] = 100;

				int dataIdx = (i+nSpike)%32;
                for (int j=0; j<s.n_chans; j++){
						int n = 0;
						if (noise>0)
							n = (rand() % (noise*2))-noise;

                        s.data[idx++] =  (trace[dataIdx]*200) + n;
                }
        }
        return s;
}

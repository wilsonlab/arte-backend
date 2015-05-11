#include "netcom.h"
#include "datapacket.h"
#include <math.h>
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

lfp_bank_net_t genFakeLfp();
timeval then, now, start,t;


int freq = 5;
int lfpSampRate = 1000;
int range = 1000;
int nChan = 8;
int noise = 0;
uint32_t seqNum = 0;

double const pi = 3.14159265;

int main(int argc, char* argv[]){

	if (argc<3)
	{
		std::cout<<"Usage: txArteLFP ip port [freq] [sampRate] [range] [nChan] [noise]"<<std::endl;
		return 0;
	}
	char *host = argv[1];
	int port = atoi(argv[2]);

	if (argc>3)
		freq =  atoi(argv[3]);

	if (argc>4)
		lfpSampRate = atoi(argv[4]);

	if (lfpSampRate<1000)
	{
		std::cout<<"Sampe rate must be atleast 1000"<<std::endl;
		return 0;
	}

	if (argc>5)
		range = atoi(argv[5]);

	if (argc>6)
		nChan = atoi(argv[6]);

	if (argc>7)
		noise = atoi(argv[7]);

	srand( time(NULL));

	int sleep =  (1e6)/1000; // LFP Packets are sent at 1kHz

	std::cout<<"Sending LFP to:"<<host<<" on port:"<<port<<std::endl;
	std::cout<<"\tSignal Freq:\t"<<freq<<" Hz"<<std::endl;
	std::cout<<"\tSamp rate:\t"<< lfpSampRate<<" Hz"<<std::endl;
	std::cout<<"\tRange +/-:\t"<<range<<" AD units"<<std::endl;
	std::cout<<"\tN Channels:\t"<<nChan<<std::endl;
	std::cout<<"\tNoise Floor:\t"<< noise<<" AD units"<<std::endl;

	NetComDat net = NetCom::initUdpTx(host,port);

	lfp_bank_net_t l;
	gettimeofday(&then,NULL);
	long dt;

	do{
		gettimeofday(&then, NULL);
		l = genFakeLfp();
		NetCom::txWave(net, &l);
//		std::cout<<"Txing LFP"<<std::endl;

		gettimeofday(&now,NULL);
		dt = (now.tv_usec - then.tv_usec);
		if (dt>1000)
			std::cout<<"dt:"<<dt<<std::endl;
		else
			usleep(1000-dt); // try to space the txing out to almost exactly 1ms
	}while(true);

}

lfp_bank_net_t genFakeLfp(){

        lfp_bank_net_t l;
        l.ts = 100000;
        l.name = 75;
        l.n_chans = nChan;
        l.n_samps_per_chan = lfpSampRate/1000;
        l.samp_n_bytes = 2;

        int idx=0;

		int v;

        for (int i=0; i<l.n_samps_per_chan; i++)
        {
                for (int j=0; j<l.n_chans; j++){
					gettimeofday(&t, NULL);
					v = (int)(sin( freq * (j*1e6 / (pi*l.n_chans) + (t.tv_usec - start.tv_usec) )/1e6 * 2 * pi) * range);
//	                    l.data[idx++] =  v + (rand() % noise);
					l.data[idx++] = v;
//					std::cout<<v<<"\t";
                }
        }
//		std::cout<<std::endl;
		l.seq_num = seqNum++;

		char buff[1024];
		int buffSize = 1024;
		bzero(buff, 1024);
		lfp_bank_net_t s;
		waveToBuff(&l, buff, &buffSize, true);
		buffToWave(&s, buff, true); 

        return l;
}

double voltage(int freq, double t){
	return sin(freq*t*2*pi);
}

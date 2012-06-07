#include "netcom.h"
#include "datapacket.h"
#include <iostream>

spike_t makeSpike(uint32_t ts, uint8_t nC, uint8_t nS){
	spike_t s;
	s.ts = 12345;
	s.src = 75;
	s.filt = 76;
	s.nChan = 4;
	s.nSamp = 32;		

	s.gain.resize(s.nChan);
	for (int i=0; i<s.nChan; i++)
		s.gain[i] = 1000;	

	s.thold.resize(s.nChan);
	for (int i=0; i<s.nChan; i++)
		s.thold[i] = 75;

	s.data.resize(s.nChan*s.nSamp);
	for (int i=0; i<(s.nChan*s.nSamp); i++)	
		s.data[i]=i%s.nSamp;

	return s;
}

void dispSpike(spike_t s){
	
	std::cout<<"Ts:"<<s.ts;
	std::cout<<"\tsrc:"<<(int) s.src;
	std::cout<<"\tfilt:"<<(int) s.filt;
	std::cout<<"\tnChan:"<<(int) s.nChan;
	std::cout<<"\tnSamp:"<<(int) s.nSamp<<std::endl;

	std::cout<<"Gains:";

	for (int i=0; i<s.nChan; i++)
		std::cout<<"\t"<<s.gain.at(i);
	
	std::cout<<"\tTholds:";
	for (int i=0; i<s.nChan; i++)
		std::cout<<"\t"<<s.thold[i];

	std::cout<<std::endl<<std::endl;
	std::cout<<"Data:";
	for (int i=0; i<(s.nChan*s.nSamp); i++){
		std::cout<<"\t"<<s.data[i];
		if (s.data[i]%10==9 || i%s.nSamp==s.nSamp-1)
			std::cout<<std::endl;
	}
	std::cout<<std::endl;
}

int main(){

	spike_t s = makeSpike(1000, 4, 32);
	char buff[500];
	bzero(buff,500);

	std::cout<<"Created Spike"<<std::endl;	
	std::cout<<"Spike-->Buffer"<<std::endl;	
	spikeToBuff(&s, buff, 500);
	std::cout<<"Buffer-->Spike"<<std::endl;	
	spike_t t = buffToSpike(buff, 500);
	
	std::cout<<"---------------------INPUT------------------"<<std::endl;
	dispSpike(s);
	std::cout<<"---------------------OUTPUT-----------------"<<std::endl;
	dispSpike(t);
}

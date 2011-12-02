
#include "TetrodeSource.h"

TetrodeSource::TetrodeSource(){
	nSpikesRead = 0;	
	readingPackets = false;
}

spike_net_t TetrodeSource::getNextSpike(){
	pthread_mutex_lock(&mtx);

	if (spikebuffer->empty()){
		pthread_mutex_unlock(&mtx);
		return null;
	}

	else{
		spike_net_t s = spikebuffer->front();
		spikebuffer->pop();
		pthread_mutex_unlock(&mtx);

		return s;
	}		
}

void TetrodeSource::enableSource(char * p){
	std::cout<<"Initializing a new TetrodeSource on port:"<<p<<"\n"<<std::flush;
	port = p;
	
	pthread_t netThread;
	net = NetCom::initUdpRx(host,port);
	pthread_create(&netThread, NULL, networkThreadFunc, this);
}
void TetrodeSource::disableSource(){
	readingPackets = false;
	
}

void TetrodeSource::getNetworkSpikePacket(){
	readingPackets = true;
	
	while(readingPackets){
		
		NetCom::rxSpike(net, &tmpSpike);
	
		pthread_mutex_lock(&mtx);
		
		while (spikebuffer->size()>MAX_SPIKE_BUFF_SIZE)
			spikebuffer->pop();
		
		spikebuffer->push(&tmpSpike);
		pthread_mutex_unlock(&mtx);
		
		nSpikeRead++;
	}
}

void TetrodeSource::getBufferSize(){
	return spikebuffer->size();
}
void TetrodeSource::flush(){
	pthread_mutex_lock(&mtx);
	std::queue<spike_net_t> *empty;
	spikebuffer = empty;
	pthread_mutex_unlock(&mtx);
	
}
void *networkThreadFunc(void *ptr){
	TetrodeSource *tp = (TetrodeSource*) ptr;
	tp->getNetworkSpikePacket();
}





#include "TetrodeSource.h"

TetrodeSource::TetrodeSource(){
	nSpikesRead = 0;	
	readingPackets = false;
}

bool TetrodeSource::getNextSpike(spike_net_t *spike){
	pthread_mutex_lock(&mtx);

	if (spikebuffer->empty()){
		pthread_mutex_unlock(&mtx);
		return false;
	}

	else{
		spike_net_t s = spikebuffer->front();
		spike = &s;
		spikebuffer->pop();
		pthread_mutex_unlock(&mtx);

		return true;
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

void TetrodeSource::getNetworkSpikePackets(){
	readingPackets = true;
	
	while(readingPackets){
		spike_net_t newSpike;
		NetCom::rxSpike(net, &newSpike);
	
		pthread_mutex_lock(&mtx);
		
		while (spikebuffer->size()>MAX_SPIKE_BUFF_SIZE)
			spikebuffer->pop();
		
		spikebuffer->push(newSpike);
		pthread_mutex_unlock(&mtx);
		
		nSpikesRead++;
	}
}

int TetrodeSource::getBufferSize(){
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
	tp->getNetworkSpikePackets();
}




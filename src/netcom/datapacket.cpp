#include <iostream>
#include "datapacket.h"

void printBuff(char* buff, int blen){
        char val;
        for (int i=0; i<blen; i++){
                val = *(buff+i);
                std::cout<<"\\"<<(int)val;           
        }
	std::cout<<std::endl;
}


/*------------- TIME ------------*/
void tsToBuff(timestamp_t* t, char* buff, int blen){
//	if (blen<6)
//		std::cout<<"ERROR: Buffer is too short"<<std::endl;
	timestamp_t ts = hton32(*t);	
	memcpy(buff+2, &ts, 4);
}

timestamp_t buffToTs(char *buff, int blen){

//	if (blen<6)
//		std::cout<<"Error buffer is too short"<<std::endl;

	timestamp_t s;
	memcpy(&s, buff+2, 4);	
	s = ntoh32(s);
	return s;
}
/*------------- SPIKE ------------*/
void spikeToBuff(spike_t* s, char* buff, int blen){
	std::cout<<"spikeToBuff(), isn't checking the buffer size, implement this!"<<std::endl;

	buff[0] = typeToChar(NETCOM_UDP_SPIKE); 
	buff[1] = 0; 

	uint32_t tmp32 = hton32(s->ts);
	uint16_t tmp16 = 0;
	memcpy(buff+2, &tmp32,4);
	memcpy(buff+6, &(s->src), 1);
	memcpy(buff+7, &(s->filt), 1);
	memcpy(buff+8, &(s->nChan),1);
	memcpy(buff+9, &(s->nSamp),1);

	int i, ind=10;

	for (i=0; i < s->nChan; i++){
		tmp16 = hton16(s->gain[i]);
		memcpy(buff+ind, &tmp16, 2);
		ind += 2;
	}

	for (i=0; i < s->nChan; i++){
		tmp16 = hton16(s->thold[i]);
		memcpy(buff+ind, &tmp16, 2);
		ind += 2;
	}

	for (i=0; i< (s->nChan)*(s->nSamp); i++){
		tmp16 = hton16(s->data[i]);
		memcpy(buff+ind, &tmp16, 2);
		ind += 2;
	}
}

spike_t buffToSpike( char* buff, int blen){
//TODO IMPLEMENT
	spike_t s;
	
	uint32_t tmp32;
	uint16_t tmp16;

	memcpy(&tmp32, buff+2, 4);
	s.ts = ntoh32(tmp32);

	memcpy(&(s.src), buff+6,1);
	memcpy(&(s.filt),buff+7, 1);
	memcpy(&(s.nChan), buff+8, 1);
	memcpy(&(s.nSamp), buff+9, 1); 
	
	int i, ind=10;

	s.gain.resize(s.nChan);
	for (i=0; i<s.nChan; i++){
		memcpy(&tmp16, buff+ind, 2);
		s.gain[i] = ntoh16(tmp16);
		ind += 2;
	}
	
	s.thold.resize(s.nChan);
	for (i=0; i<s.nChan; i++){
		memcpy(&tmp16, buff+ind, 2);
		s.thold[i] = ntoh16(tmp16);
		ind += 2;
	}

	s.data.resize(s.nChan*s.nSamp);
	for (i=0; i<(s.nChan * s.nSamp); i++){
		memcpy(&tmp16, buff+ind,2);
		uint16_t tmp2 = ntoh16(tmp16);
		s.data[i] = ntoh16(tmp16);
		ind += 2;
	}
	return s;
}

/*------------- WAVE ------------*/
void waveToBuff(lfp_bank_net_t* lfp, char* buff, int blen){
  //TODO IMPLEMENT
  //printf("waveToBuff\n");
  
  int s;

  int cursor = 2;
  int16_t name_tx =              hton16(*(lfp->name));
  uint16_t ts_tx =               hton32(*(lfp->ts));
  uint16_t n_chans_tx =          hton16(*(lfp->n_chans));
  uint16_t n_samps_per_chan_tx = hton16(*(lfp->n_samps_per_chan));
  uint16_t bytes_per_samp_tx =   hton16(*(lfp->samp_n_bytes));
  rdata_t data_tx[8100];  // find out the right minimum size for this array
  int16_t gain_tx[128];

  // flip all the elements in data
  // THIS ASSUMES 16 BIT SAMPLES
  for(s = 0; s < *(lfp->n_chans) * *(lfp->n_samps_per_chan); s++){
    data_tx[s] = hton16(lfp->data[s]);
  }
  for(s = 0; s < *(lfp->n_chans); s++){
    gain_tx[s] = hton16(lfp->gains[s]);
  }

  buff[0] = typeToChar(NETCOM_UDP_LFP);
  buff[1] = 0;

  memcpy(buff+cursor, &ts_tx                          , 4);
  cursor += 4;
  memcpy(buff+cursor, &name_tx                        , 2);
  cursor += 2;
  memcpy(buff+cursor, &n_chans_tx                     , 2);
  cursor += 2;
  memcpy(buff+cursor, &n_samps_per_chan_tx            , 2);
  cursor += 2;
  memcpy(buff+cursor, &bytes_per_samp_tx, 2);
  cursor += 2;
  memcpy(buff+cursor, &data_tx                        , (s=(*(lfp->n_chans) * *(lfp->n_samps_per_chan) * *(lfp->samp_n_bytes))));
  cursor += s;
  memcpy(buff+cursor, &gain_tx                        , (s=(*(lfp->n_chans) * 2)));
  cursor += s;

  buff[cursor] = '\0';

}

wave_t buffToWave(char* buff, int blen){
//TODO IMPLEMENT

	wave_t w;
	return w;
}
 

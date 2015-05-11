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

void tsToBuff(timestamp_t* t, char* buff, int blen, bool c){
//	if (blen<6)
//		std::cout<<"ERROR: Buffer is too short"<<std::endl;
   timestamp_t ts = hton32c(*t,c);	
   memcpy(buff+2, &ts, 4);
}

timestamp_t buffToTs(char *buff, int blen, bool c){

//	if (blen<6)
//		std::cout<<"Error buffer is too short"<<std::endl;
	timestamp_t s;
	memcpy(&s, buff+2, 4);	
	s = ntoh32c(s,c);
	return s;
}
/*------------- SPIKE ------------*/
void spikeToBuff(spike_net_t* s, char* buff, int *buff_len, bool c){
  //std::cout<<"spikeToBuff(), isn't checking the buffer size, implement this!"<<std::endl;
  buff[0] = typeToChar(NETCOM_UDP_SPIKE); 
  buff[3] = 0; 
  
  uint32_t ts_tx = hton32c(s->ts, c);
  uint16_t name_tx = hton16c(s->name, c);
  uint16_t n_chans_tx = hton16c(s->n_chans, c);
  uint16_t n_samps_per_chan_tx = hton16c(s->n_samps_per_chan, c);
  uint16_t samp_n_bytes_tx = hton16c(s->samp_n_bytes, c);
  rdata_t  data_tx[MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT];
  int16_t  gains_tx[MAX_FILTERED_BUFFER_N_CHANS];
  rdata_t  thresh_tx[MAX_FILTERED_BUFFER_N_CHANS];
  uint16_t trig_ind_tx = hton16c(s->trig_ind, c);
  uint32_t seq_num_tx = hton32c(s->seq_num, c);

   for(int i = 0; i < s->n_chans * s->n_samps_per_chan; i++){
     // ASSUMES 16-bit data!
     data_tx[i] = hton16c(s->data[i], c);
   }

   for(int i = 0; i < s->n_chans; i++){
     gains_tx[i] = hton16c(s->gains[i], c);
     thresh_tx[i] = hton16c(s->thresh[i], c);
   }

  uint16_t cursor = 4;  // start offset 4 bytes (first is packet type, 2nd and 3rd are buff_size 4th is \0
  uint16_t cursor_tx;
  
   memcpy(buff+cursor, &ts_tx,               4);
 cursor += 4;
   memcpy(buff+cursor, &name_tx,             2);
   cursor += 2;
   memcpy(buff+cursor, &n_chans_tx,          2);
   cursor += 2;
   memcpy(buff+cursor, &n_samps_per_chan_tx, 2);
   cursor += 2;
   memcpy(buff+cursor, &samp_n_bytes_tx,        2);
   cursor += 2;
   memcpy(buff+cursor, &data_tx,             (s->n_chans * s->n_samps_per_chan * s->samp_n_bytes));
   cursor += (s->n_chans * s->n_samps_per_chan * s->samp_n_bytes);
   memcpy(buff+cursor, &gains_tx,            (2 * s->n_chans));
   cursor += 2*s->n_chans;
   memcpy(buff+cursor, &thresh_tx,           (2 * s->n_chans));
   cursor += 2*s->n_chans;
   memcpy(buff+cursor, &trig_ind_tx,         2);
   cursor += 2;
   memcpy(buff+cursor, &seq_num_tx,          4);
   cursor += 4;

   cursor_tx = hton16c(cursor,c);
   memcpy(buff+1,      &cursor_tx,           2);

  buff[cursor] = '\0';
  *buff_len = cursor;

}

void buffToSpike( spike_net_t *s, char* buff, bool c){
  std::cout << "try 1" << std::endl;
  int cursor = 4;
  std::cout << "try 2" << std::endl;
  memcpy( &(s->ts),        buff+cursor, 4);
  std::cout << "try 3" << std::endl;
  s->ts = ntoh32c(s->ts, c);
  std::cout << "try 4" << std::endl;
  cursor += 4;
  
  std::cout << "try 5" << std::endl;
  memcpy( &(s->name),      buff+cursor, 2);
  std::cout << "try 6" << std::endl;
  s->name = ntoh16c(s->name, c);
  std::cout << "try 7" << std::endl;
  cursor += 2;

  std::cout << "try 8" << std::endl;
  memcpy( &(s->n_chans),   buff+cursor, 2);
  std::cout << "try 9" << std::endl;
  s->n_chans = ntoh16c(s->n_chans, c);
  std::cout << "try 10" << std::endl;
  cursor += 2;

  std::cout << "try 11" << std::endl;
  memcpy( &(s->n_samps_per_chan), buff+cursor, 2);
  std::cout << "try 12" << std::endl;
  s->n_samps_per_chan = ntoh16c(s->n_samps_per_chan, c);
  std::cout << "try 13" << std::endl;
  cursor += 2;

  std::cout << "try 14" << std::endl;
  memcpy( &(s->samp_n_bytes),     buff+cursor, 2);
  std::cout << "try 15" << std::endl;
  s->samp_n_bytes = ntoh16c(s->samp_n_bytes, c);
  std::cout << "try 16" << std::endl;
  cursor += 2;


  std::cout << "try 17" << std::endl;
  int n_total_samps = s->n_chans * s->n_samps_per_chan;
  std::cout << "try 18" << std::endl;
  int data_bytes = n_total_samps * s->samp_n_bytes;
  std::cout << "try 19" << std::endl;
  int gain_bytes = s->n_chans * 2;    // 2 byte datatype for gain: int16
  std::cout << "try 20" << std::endl;
  int thresh_bytes = s->n_chans * 2;  // 2 byte datatype for thresh: rdata_t
  std::cout << "n_total_samps: " << n_total_samps << " n_samps_per_chan " << s->n_samps_per_chan << " n_chans: " << s->n_chans << " samp_n_bytes: " << s->samp_n_bytes << std::endl; 

  std::cout << "try 21" << std::endl;
  std::cout << "data_bytes: " << data_bytes << std::endl;
  memcpy( &(s->data),           buff+cursor, data_bytes);
  std::cout << "try 22" << std::endl;
  for(int n = 0; n < n_total_samps; n++){
    std::cout << "try 23" << std::endl;
  s->data[n] = ntoh16c(s->data[n], c);
 
  }
 

 std::cout << "try 24" << std::endl;
  cursor += data_bytes;
 
  std::cout << "try 25" << std::endl;
    memcpy( &(s->gains),         buff+cursor, gain_bytes);
    std::cout << "try 26" << std::endl;
    cursor += gain_bytes;
    std::cout << "try 27" << std::endl;
    memcpy( &(s->thresh),        buff+cursor, thresh_bytes);
    std::cout << "try 28" << std::endl;
    cursor += thresh_bytes;

    std::cout << "try 29" << std::endl;
    for(int n = 0; n < s->n_chans; n++){
      std::cout << "try 30" << std::endl;
         s->gains[n] = ntoh16c(s->gains[n], c);
    std::cout << "try 31" << std::endl;
       s->thresh[n] = ntoh16c(s->thresh[n], c);
  }

    std::cout << "try 32" << std::endl;
       memcpy( &(s->trig_ind),      buff+cursor, 2); //
   std::cout << "try 33" << std::endl;
   s->trig_ind = ntoh16c(s->trig_ind, c);
   std::cout << "try 34" << std::endl;
   cursor += 2;
  
   std::cout << "try 35" << std::endl;
     memcpy( &(s->seq_num),       buff+cursor, 4); //
   std::cout << "try 36" << std::endl;
   s->seq_num = ntoh32c(s->seq_num, c);
   std::cout << "try 37" << std::endl;
   cursor += 4;
   std::cout << "try 38" << std::endl;
}

void commandToBuff(command_t *command, char* buff, int *blen, bool c){
  uint16_t cursor = 4;
  uint16_t cursor_tx;
  uint16_t n_char_tx;
  buff[0] = typeToChar(NETCOM_UDP_COMMAND);
  buff[3] = 0;

  int command_len = strlen(command->command_str);

  n_char_tx = ntoh16c( strlen(command->command_str), c);

  memcpy(buff+cursor, &n_char_tx, 2);
  cursor += 2;
  
  memcpy(buff+cursor, &(command->command_str[0]), command_len);
  cursor += command_len;

  *blen = cursor;
  
  cursor_tx = hton16c(cursor, c);
  memcpy( buff+1, &cursor_tx, 2 );
}

void buffToCommand(command_t *command, char* buff, bool c){
  uint16_t cursor = 4;
  uint16_t n_char;
  uint16_t buff_len;
  memcpy( &buff_len, buff+1, 2 );
  buff_len = ntoh16c(buff_len, c);

  memcpy( &n_char, buff+cursor, 2);
  n_char = ntoh16c(n_char, c);
  cursor += 2;

  memcpy( command->command_str, buff+cursor, n_char );
  cursor += n_char;
  command->n_char = n_char;
  command->command_str[n_char] = '\0';

}

/*------------- WAVE ------------*/
void waveToBuff(lfp_bank_net_t* lfp, char* buff, int *blen, bool c){
  int s;

  uint16_t cursor = 4;
  uint16_t cursor_tx;

  int16_t name_tx =              hton16c(lfp->name, c);
  uint32_t ts_tx =               hton32c(lfp->ts, c);
  uint16_t n_chans_tx =          hton16c(lfp->n_chans, c);
  uint16_t n_samps_per_chan_tx = hton16c(lfp->n_samps_per_chan, c);
  uint16_t bytes_per_samp_tx =   hton16c(lfp->samp_n_bytes, c);
  rdata_t data_tx[MAX_FILTERED_BUFFER_TOTAL_SAMPLE_COUNT];  // find out the right minimum size for this array
  int16_t gain_tx[MAX_FILTERED_BUFFER_N_CHANS];
  uint32_t seq_tx = 			 hton32c(lfp->seq_num, c);

  // flip all the elements in data
  // THIS ASSUMES 16 BIT SAMPLES
  for(s = 0; s < lfp->n_chans * lfp->n_samps_per_chan; s++){
    data_tx[s] = hton16c(lfp->data[s], c);
  }
  for(s = 0; s < lfp->n_chans; s++){
    gain_tx[s] = hton16c(lfp->gains[s], c);
  }

  buff[0] = typeToChar(NETCOM_UDP_LFP);
  buff[3] = '\0';

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
  // is this next line right?  &data_tx or data_tx ???
  memcpy(buff+cursor, &data_tx						  , (s=(lfp->n_chans * lfp->n_samps_per_chan * lfp->samp_n_bytes)));
  cursor += s;
  memcpy(buff+cursor, &gain_tx                        , (s=(lfp->n_chans * 2)));
  cursor += s;
  memcpy(buff+cursor, &seq_tx						  ,  4);
  cursor += 4;

  // NEW
  cursor_tx = hton16c(cursor, c);
  memcpy(buff+1,     &cursor_tx, 2);

  buff[cursor] = '\0';
  *blen = cursor;

  if(false){
    printf("incoming ts: %d.  ts after swap: %d",lfp->ts, ts_tx);
  }

  if(false){
    printf("buffer being written: ");
    for(int n = 0; n < *blen; n++)
      printf("%d:%c\n ",n,buff[n]);
  } 
}

void buffToWave(lfp_bank_net_t *lfp, char* buff, bool c){
  //TODO IMPLEMENT
  int cursor = 4;  // skip over the packet_type field


  memcpy( &(lfp->ts), buff+cursor, 4);
  lfp->ts = ntoh32c(lfp->ts, c);
  cursor += 4;

  memcpy( &(lfp->name), buff+cursor,  2);
  lfp->name = ntoh16c(lfp->name, c);
  cursor +=2;
  
  memcpy( &(lfp->n_chans), buff+cursor, 2);
  lfp->n_chans = ntoh16c(lfp->n_chans, c);
  cursor +=2;

  memcpy( &(lfp->n_samps_per_chan), buff+cursor, 2);
  lfp->n_samps_per_chan = ntoh16c(lfp->n_samps_per_chan, c);
  cursor += 2;

  memcpy( &(lfp->samp_n_bytes), buff+cursor, 2);
  lfp->samp_n_bytes = ntoh16c(lfp->samp_n_bytes, c);
  cursor += 2;
  
  int n_total_samps = lfp->n_chans * lfp->n_samps_per_chan;
  int data_bytes = lfp->n_chans * lfp->n_samps_per_chan * lfp->samp_n_bytes;
  int gain_bytes = lfp->n_chans * 2; // gain type is int16_t
  int seq_bytes = 4; //uint32_t

  memcpy( &(lfp->data), buff+cursor, data_bytes);
  for(int n = 0; n < n_total_samps; n++)
    lfp->data[n] = ntoh16c(lfp->data[n], c);
  cursor += data_bytes;

  memcpy( &(lfp->gains), buff+cursor, gain_bytes);
  for(int n = 0; n < lfp->n_chans; n++)
    lfp->gains[n] = ntoh16c(lfp->gains[n], c);
  cursor += gain_bytes;

  memcpy( &(lfp->seq_num),       buff+cursor, 4);
  lfp->seq_num = ntoh32c(lfp->seq_num, c);
  cursor += 4;
}
 

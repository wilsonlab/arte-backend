#include "unp.h"
#include "netcom.h"
#include "datapacket.h"


//void watch_net( char host_ip[INET6_ADDRSTRLEN], int port_num ){
//
//}

int main (int argc, char *argv[]){
 
  NetCom    my_netcom;
  NetComDat my_netcomdat;
  int port_num;
  int packet_count = 0;
  char buff[BUFFSIZE];
  lfp_bank_net_t lfp;
  int buff_len;

  if(argc != 3){
    printf("Usage: toy_wave_viewer host port\n");
    exit(1);
  }

  //  std::istringstream iss(argv[2]);
  //iss >> port_num;

  //printf("before init\n");
  //fflush(stdout);
  my_netcomdat = my_netcom.initUdpRx( argv[1], argv[2] );
  //printf("after init\n");
  //fflush(stdout);
  while(1){

    if(false){   // just get buffer as a buffer
      NetCom::rxBuff(my_netcomdat, buff, &buff_len);
    
      //printf("%c|",buff[0]);
      //fflush(stdout);
      if(false){
	printf("got buff.  length: %d.  Content: ",buff_len);
	for(int c = 0; c < buff_len; c++)
	  printf("%d:  %c\n", c, buff[c]);
	printf("/n");
      }
    }

    if(true){  //get buffer as wave packet
      lfp_bank_net_t lfp;
      //printf("before rxwave\n");
      //fflush(stdout);
      NetCom::rxWave( my_netcomdat, &lfp );
      //printf("after rxwave\n");
      //fflush(stdout);
      
      if(lfp.ts % 400 == 0){
      printf("got one. ts is: %d   ", lfp.ts);
      printf("name: %d   ", lfp.name);
      printf("n_chans: %d  ", lfp.n_chans);
      for(int s = 0; s < lfp.n_samps_per_chan; s++){
	printf("  samp%d:  ",s);
	for(int c = 0; c < lfp.n_chans; c++){
	  printf("%d ", lfp.data[ s*lfp.n_chans + c ]);
	}
      }
      printf("\n");
      }

    }
    
    if(false){
      packet_count++;
      if(packet_count % 50 == 0)
	printf("packetcount: %d\n",packet_count);
    }

    fflush(stdout);
  
  }

  return 0;

}
     

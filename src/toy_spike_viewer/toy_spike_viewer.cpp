#include "unp.h"
#include "../netcom/netcom.h"
#include "../netcom/datapacket.h"


//void watch_net( char host_ip[INET6_ADDRSTRLEN], int port_num ){
//
//}

int main (int argc, char *argv[]){
 
  NetCom    my_netcom;
  NetComDat my_netcomdat;
  int port_num;
  int packet_count = 0;
  char buff[BUFFSIZE];
  spike_net_t spike;
  int buff_len;

  if(argc != 3){
    printf("Usage: toy_wave_viewer host port\n");
    exit(1);
  }

  std::istringstream iss(argv[2]);
  iss >> port_num;

  //printf("before init\n");
  //fflush(stdout);
  
  my_netcomdat = my_netcom.initUdpRx( argv[1], port_num );
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

    if(true){  //get buffer as spike packet
      
      printf("before rxwave\n");
      fflush(stdout);
      NetCom::rxSpike( my_netcomdat, &spike );
      printf("after rxwave\n");
      fflush(stdout);
      
      if(true){
      printf("got one. ts is: %d   ", spike.ts);
      printf("name: %d   ", spike.name);
      printf("n_chans: %d  ", spike.n_chans);
      for(int s = 0; s < spike.n_samps_per_chan; s++){
	printf("  samp%d:  ",s);
	for(int c = 0; c < spike.n_chans; c++){
	  printf("%d ", spike.data[ s*spike.n_chans + c ]);
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
     

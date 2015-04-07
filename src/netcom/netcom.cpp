#include "../netcom.h"
#include "../datapacket.h"


/*
NetComDat NetCom::initUdpTx(char host[], int port){

	int sockfd; 
	sockaddr_in addr;
	hostent *he;
	int numbytes;
	int broadcast = 1;
	std::cout << "reading from port" << port << std::endl;

	if ((he=gethostbyname(host))==NULL){
	  perror("gethostname");
	  printf("gethostbyname error.\n");
	  fflush(stdout);
	  exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
	  perror("socket");
	  printf("socket error.\n");
	  fflush(stdout);
	  exit(1);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
	  perror("setsockopt (SO_BROADCAST)");
	  printf("setsockopt error\n");
	  fflush(stdout);
	  exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = *((struct in_addr *)he->h_addr);

	memset(addr.sin_zero, '\0', sizeof addr.sin_zero);
	NetComDat net;

	net.sockfd = sockfd;
	net.addr_in =  addr;
	return net;
}

*/

NetComDat NetCom::initUdpRx(char host[], char const *port){ // ADDED CONST


	printf("initializing NetComDat for UDP-RX\n");
	int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

	NetComDat net;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE  |  AI_NUMERICSERV; // use my IP

	std::cout<<"Listening to port:"<<port<<" from IP:"<<host<<std::endl;

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
			std::cout<<"RV VALUE:"<<rv<<std::endl;
	        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return net;
    		}

        // loop through all the results and bind to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
                if ((sockfd = socket(p->ai_family, p->ai_socktype,
                                p->ai_protocol)) == -1) {
                        perror("listener: socket");
                        continue;
                }

                if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                        close(sockfd);
                        perror("listener: bind");
                        continue;
                }

                break;
        }

        if (p == NULL) {
                fprintf(stderr, "listener: failed to bind socket\n");
                return net;
        }

        freeaddrinfo(servinfo);

	net.sockfd = sockfd;
	net.their_addr =  their_addr;
	return net;
}




int NetCom::txTs(NetComDat net, timestamp_t count, int nTx){
	std::cout<<"NetCom::txTs "<<count<<std::endl;

	char buff[6]; //  this is much faster than using new
	*buff = typeToChar(NETCOM_UDP_TIME);

	for (int i=0; i<nTx; i++)
		sendto(net.sockfd, buff, 6, 0, (struct sockaddr *)&net.addr_in, sizeof net.addr_in);
}

timestamp_t NetCom::rxTs(NetComDat net){

	char s[INET6_ADDRSTRLEN];
        char buff[6] = {'\0'};;

        int numbytes;
	sockaddr_storage their_addr = net.their_addr;
        socklen_t addr_len = sizeof (their_addr);

	sockaddr_in sa = *(struct sockaddr_in *)&their_addr;

	if ((numbytes = recvfrom(net.sockfd, &buff, 20 , 0,
                (struct sockaddr *)&their_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
        }


        printf("listener: got packet from %s\n", 
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr), s, 
		sizeof s));

        return 0;
}

int NetCom::txSpike(NetComDat net, spike_net_t *spike){

  char spike_buff[MAX_SPIKE_NET_BUFF_SIZE];
  int spike_buff_size;

  spikeToBuff(spike, spike_buff, &spike_buff_size, true);

  sendto( net.sockfd, spike_buff, spike_buff_size, 0, (sockaddr*) &(net.addr_in), sizeof( net.addr_in) );
  return 0;

}

void NetCom::rxSpike(NetComDat net, spike_net_t* spike){

  char buff[BUFFSIZE-1];
  int buff_len = 0;
  // printf("Netcom::rxSpike() calling rxBuff()\n");
  rxBuff(net, buff, &buff_len);
//  printf("Netcom::rxSpike() converting buff to spike\n");
  buffToSpike(spike, buff, true);
  //printf("Netcom::rxSpike() the buff is converted\n");
}

int NetCom::txWave(NetComDat net, lfp_bank_net_t *lfp){
  char buff[BUFFSIZE-1];
  int buff_size;
  if(  (lfp->ts >= 0 & lfp->ts <(UINT32_MAX - 10000) ) ){
    waveToBuff(lfp, buff, &buff_size, true);
    NetCom::txBuff(net, buff, buff_size);
  }
  return 0;
}

void NetCom::rxWave(NetComDat net, lfp_bank_net_t *lfp){

  char buff[BUFFSIZE-1];
  int buff_len = 0;
  rxBuff(net, buff, &buff_len);
  buffToWave(lfp, buff, true); 

}

int NetCom::txCommand(NetComDat net, command_t *the_command){
	char buff[BUFFSIZE-1];
  	int buff_len = 0;
	commandToBuff( the_command, buff, &buff_len, true );
	NetCom::txBuff(net, buff, buff_len);
}

void NetCom::rxCommand(NetComDat net, command_t *the_command){
	char buff[BUFFSIZE-1];
	int buff_len = 0;
	rxBuff(net, buff, &buff_len);
	buffToCommand(the_command, buff, true);
}

void NetCom::txBuff(NetComDat net, char *buff, int buff_len){

  if(false){  // this thing sets the buff line to all 'a'.  Kinda useful for testing listeners.
    printf("buff len: %d   content: ",buff_len);
    for(int c = 0; c < buff_len; c++){
      buff[c] = 'a';
      printf("%d: %c\n",c, buff[c]);
    }
    buff[buff_len-1] = '\0';
  }

  sendto( net.sockfd, buff, buff_len, 0,  (sockaddr*) &net.addr_in, sizeof( net.addr_in ) );

}

void NetCom::rxBuff(NetComDat net, char *buff, int *buff_len){

  char s[INET6_ADDRSTRLEN];
  int numbytes;
  sockaddr_storage their_addr = net.their_addr;
  socklen_t addr_len = sizeof(their_addr);

  sockaddr_in sa = *(sockaddr_in*)&their_addr;

  if ( (numbytes = recvfrom(net.sockfd, buff, BUFFSIZE-1, 0, (SA*)&their_addr, &addr_len)) == -1){
	
    printf("recvfrom error from rxBuff.\n");
	exit(-1);
  }

  *buff_len = numbytes;

}

void *get_in_addr(struct sockaddr *sa){
        if (sa->sa_family == AF_INET) {
                return &(((struct sockaddr_in*)sa)->sin_addr);
        }

        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
string intToString(int port){
	stringstream ss;
	s << port;
	string portStr = s.str();
	return portStr;

}
*/

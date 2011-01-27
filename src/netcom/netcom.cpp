#include "netcom.h"

NetComDat NetCom::initUdpTx(char host[], int port){

	int sockfd; 
	sockaddr_in addr;
	hostent *he;
	int numbytes;
	int broadcast = 1;
	
	if ((he=gethostbyname(host))==NULL){
		perror("gethostname");
		exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		perror("socket");
		exit(1);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
	perror("setsockope (SO_BROADCAST)");
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

NetComDat NetCom::initUdpRx(char host[], int portIn){

	std::stringstream ss;
	ss<<portIn;
	const char * port = ss.str().c_str();

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
        hints.ai_flags = AI_CANONNAME; // use my IP

	std::cout<<"Listening to port:"<<port<<" from IP:"<<host<<std::endl;
        if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                return net;
        }

        // loop through all the results and bind to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
//		std::cout<<"\t"<< p->ai_canonname<<"----"<<std::endl;
//		std::cout<<"\t"<< p->ai_addr<<"----"<<std::endl;
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

int NetCom::txSyncCount(NetComDat net, uint32_t count, int nTx){
	std::cout<<"NetCom::txSyncCount"<<std::endl;
	std::cout<<"count:\t:"<< count <<" size of count:\t" << sizeof count<< std::endl;
	count = hton32(count);

	char* msg = new char[6];
	syncCountToBuffer(count, msg, sizeof msg);	
//	memcpy(msg, count,4);
	
	for (int i=0; i<nTx; i++){
		sendto(net.sockfd, msg, strlen(msg), 0, (struct sockaddr *)&net.addr_in, sizeof net.addr_in);
	}	
}

uint32_t NetCom::rxSyncCount(NetComDat net){

	char s[INET6_ADDRSTRLEN];
        char buf[MAXBUFLEN];

        int numbytes;
	sockaddr_storage their_addr = net.their_addr;
        socklen_t addr_len = sizeof (their_addr);

	sockaddr_in sa = *(struct sockaddr_in *)&their_addr;

	if ((numbytes = recvfrom(net.sockfd, &buf, MAXBUFLEN-1 , 0,
                (struct sockaddr *)&their_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
        }

        printf("listener: got packet from %s\n",
                inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),
                        s, sizeof s));

        printf("listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        printf("listener: packet contains \"%s\"\n", buf);

        return 0;
}

void *get_in_addr(struct sockaddr *sa){
        if (sa->sa_family == AF_INET) {
                return &(((struct sockaddr_in*)sa)->sin_addr);
        }

        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void NetCom::syncCountToBuffer(uint32_t count, char *buff, unsigned int blen){

	if (blen<6){
		std::cout<<"Netcom::sync2Buffer(), Buffer length is too short, ERROR!"<<blen<<std::endl;
		return;
	}
	buff[0] = (char) typeToChar(NETCOM_UDP_SYNC);
	buff[1] = '\0';

	count = hton32(count);
        char* msg = (char*) (&count);

	for(int i=0; i<4; i++)
		buff[2+i] = msg[i];

}

uint32_t NetCom::bufferToSyncCount(char *buff, unsigned int blen){
	if (blen<6){
                std::cout<<"Netcom::buffer2Sync(), Buffer length is too short, ERROR!"<<std::endl;
                return 0;
        }
	char msg[4] = "\0";
	for (int i=0; i<4; i++)
		msg[i] = buff[2+i];


	int32_t count = *((int32_t*) &msg);
	count = ntoh32(count);
	return count;	

}


#include "netcom.h"



int NetCom::txSyncCount(char host[], int port, uint32_t count, int nTx){
	int sockfd; 
	struct sockaddr_in addr;
	struct hostent *he;
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


	// TO DO figure out how to package up a 32bit unsigned int as a char array! look at 
	// erics code to figure out how he did this!
	
	count = hton32(count);
	char* msg = (char*) &count;
	
	for (int i=0; i<nTx; i++){
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&addr, sizeof addr);
	}	
}

uint32_t NetCom::rxSyncCount(int port){

}

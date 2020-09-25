#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#define SA struct sockaddr 

// structure to transfer msgs
typedef struct Message{
    int type;
    int len;
    char msg[100];
}Message;

// Function to make tcp communtication
int func(int sockfd) 
{ 
	Message mesg; //variable to Message structure to store messages
	sprintf(mesg.msg,"RequestforPortnumber"); // making request msg
	mesg.type = 1 , mesg.len = strlen(mesg.msg); //building msg structure
	int port = -1;
	write(sockfd, &mesg, sizeof(mesg)); // sending type-1 msg to server
	bzero(&mesg, sizeof(mesg)); // clearing mesg to re-use it
	read(sockfd, &mesg, sizeof(mesg)); // recieve type-2 msg from server
	if(mesg.type == 2){ // check whether the recieved msg of type-2
		port = atoi(mesg.msg);
		printf("\"type:%d\tlen:%d\tport:%d\"\t",mesg.type, mesg.len, port);
	} 
    return port;
} 

// Function to make communication through UDP
void udp(char *ip,int port){
    int sd, n,len;
    char msg[100] = "Scooby-Dooby"; // type-3 message
	Message mesg; // structure to store Message
	mesg.type = 3, //set msg type
	strncpy(mesg.msg,msg,100); // store type-3 msg in structure
	mesg.len = (sizeof(mesg.msg)); //set mesg len
    struct sockaddr_in addr; 
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){ //create udp socket
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    sendto(sd, &mesg, sizeof(mesg), MSG_CONFIRM, (const struct sockaddr *)&addr, sizeof(addr)); //send type-3 msg to server
	len = sizeof(addr); 
    n = recvfrom(sd, &mesg, sizeof(mesg), 0, (struct sockaddr *)&addr, &len); // recv type-4 msg from server
	if(mesg.type == 4) // check whether msg recieved is type-4 or not
		printf("\"type:%d\tlen:%d\tmsg:%s\" \n",mesg.type,mesg.len,mesg.msg);
    else printf("Type 4 msg not recieved!!\n");
	close(sd); //close udp
}

int main(int argc, char *argv[]) 
{ 
	// check wheteher all arguments are passed or not
	if(argc < 3){
		printf("Pass server ipaddress and port number as input!!\n");
		return 0;
	}
	int sockfd, connfd; // socket descriptors
	struct sockaddr_in servaddr, cli; 

	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	}  

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(argv[1]); 
	servaddr.sin_port = htons(atoi(argv[2])); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		perror("connection with the server failed"); 
		exit(EXIT_FAILURE); 
	} 

	int port = func(sockfd); // get the udp port from server via tcp connection
	if(port == 0) {
		printf("Type2 msg not recieved!!\n");
		return 0;
	}
	close(sockfd); // close tcp socket
    udp(argv[1],port); // initialize udp
} 

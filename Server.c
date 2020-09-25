#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include<sys/wait.h> 

#define MAX 300
#define BACKLOG 3
#define port  8888

// structure to transfer msgs
typedef struct Message{
    int type;
    int len;
    char msg[100];
}Message;

int main(int argc, char *argv[]){

	// check wheteher all arguments are passed or not
    if(argc < 2){
        printf("Port isn't specified\n");
        return 0;
    }
    int next_port = atoi(argv[1]);
    if(next_port >= 8888) next_port = port + 2;
    else next_port = port;
    
    int main_socket, adrrlen, sd; 
    struct sockaddr_in addr;

    Message mesg; // to store msgs sent and recieved

    //  udp variables
    int udp_sd;
    struct sockaddr_in udp_sa, udp_ca;
    
    int opt = 1;

    //create a main TCP socket
    if((main_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Main Socket Creation Failed");
        exit(EXIT_FAILURE);
    }

    //Set socket_adrress parameters
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[1]));

    // set socket options
    if( setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // bind socket to address
    if(bind(main_socket,(struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("Binding Failure");
        exit(EXIT_FAILURE);
    }

    // listen to tcp socket
    if(listen(main_socket, BACKLOG) < 0){
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }
    else{
        printf("listening...\n");
    }

    adrrlen = sizeof(addr);

    while(1){
        //accept the tcp connection
        sd = accept(main_socket, (struct sockaddr *)&addr, (socklen_t*)&adrrlen);
        // build address for udp
        udp_sa.sin_family = AF_INET;
        udp_sa.sin_addr.s_addr = INADDR_ANY;
        udp_sa.sin_port = htons(next_port);

        // create udp socket
        if((udp_sd = socket(AF_INET,SOCK_DGRAM, 0))<0){
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }

        
        //bind udp socket to address
        while( bind(udp_sd, (const struct sockaddr *)&udp_sa, sizeof(udp_sa)) <0){
            next_port += 2;
            udp_sa.sin_port = htons(next_port);
        }


        if(fork() == 0){ //if chlid process
            close(main_socket); // close main tcp socket in child
            read(sd, &mesg, sizeof(mesg)); // read mesg from client
            getpeername(sd , (struct sockaddr*)&addr , (socklen_t*)&adrrlen);
            int len = sizeof(udp_ca); 
            //check if mesg is type-1 or not
            if(mesg.type == 1){
            	// print mesg recived by client
            	printf("\"type:%d\tlen:%d\tmsg:%s\"\t",mesg.type, mesg.len, mesg.msg);
                bzero(&mesg,sizeof(mesg)); //clear memory of structure to re-use it
                mesg.type = 2, mesg.len = 4;
                sprintf(mesg.msg,"%d",next_port); //build type 2 mesg
                send(sd, &mesg, sizeof(mesg), 0); //send type 2 mesg
            }

            close(sd); // close tcp connection
            int n = recvfrom(udp_sd, &mesg, sizeof(mesg), 0, (struct sockaddr *)&udp_ca, &len);
            if(mesg.type == 3){
                printf("\"type:%d\tlen:%d\tmsg:%s\"\n",mesg.type, mesg.len, mesg.msg);
                bzero(&mesg,sizeof(mesg));//clear memory of structure to re-use it
                sprintf(mesg.msg,"Bye-bye Client!!"); // build type-4 mesg
                mesg.type = 4, mesg.len = strlen(mesg.msg);
                sendto(udp_sd, &mesg, sizeof(mesg), MSG_CONFIRM, (const struct sockaddr *)&udp_ca, sizeof(udp_ca)); //send type-4 mesg to client
            }
            else printf("type-3 mesg not recieved properly!!\n");
            close(udp_sd);
            exit(0);
        }
        close(sd); // close client tcp socket used by child in parent
        close(udp_sd); // close client udp socket used by child in parent
        signal(SIGCHLD,SIG_IGN); // ignore status of child
        next_port += 2;
    }
    return 0;
}

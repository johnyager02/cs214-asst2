#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include "../stringFunc.h"
#include "../recursiveD.h"
#include "../sendAndReceive.h"
#include "../manifestFunc.h"
#include "../WTF.h"
#define MAX 80 
#define SA struct sockaddr 

  
int main(int argc, char** argv) { 
    char* command = argv[1];

    char IP[40];
    int PORT;
    if(compareString("configure", command) == 0){
        if(argc != 4){
            printf("Error: config command takes IP and port as additional arguments\n");
            return 1;
        }
        int configFile = open("../.Config", O_WRONLY | O_CREAT, 00644);
        char* configString = malloc(strlen(argv[2]) + 2);
        strcpy(configString, argv[2]);
        configString[strlen(argv[2])] = ' ';
        configString[strlen(argv[2]) + 1] = '\0';
        configString = appendToStr(configString, argv[3]);
        write(configFile, configString, strlen(configString));
        printf("Configuration file set up. This IP address and Port will be used for future commands.");
        return 0;
    }
    else{
        //attempt to open Config file
        char* config = getFileContents("../.Config");

        sscanf(config, "%s %d", &IP, &PORT);
        printf("%s %d\n", IP, PORT);
    }
    int connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else{
        printf("Socket successfully created..\n"); 
    }

    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(IP); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    // close the socket 
    close(sockfd); 
} 

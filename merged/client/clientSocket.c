#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
int makeSocket();

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
#define PORT 8080 
#define SA struct sockaddr 
void func(int sockfd) 
{ 
    char* buff = (char*) mallocStr(1025);
    memset(buff, '\0',1025*sizeof(char)); 
    int n; 
    int numBytesRead = 0;
    int totalReadInBytes = 0;
    int currentBufferSize = 1024;
    for (;;) { 
        memset(buff, '\0', currentBufferSize*sizeof(char)); 
        printf("Enter the string : \n"); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 

        write(sockfd, buff, 10*sizeof(char));
        // write(sockfd, buff, 10*sizeof(char)); 
        // bzero(buff, currentBufferSize); 
        // read(sockfd, buff, sizeof(buff)); 
        // printf("From Server : %s", buff); 
        // if ((strncmp(buff, "exit", 4)) == 0) { 
        //     printf("Client Exit...\n"); 
        //     break; 
        // } 
    } 
} 
  
int main() 
{ 
    int connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    int reuseSocket = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseSocket, sizeof(int)) < 0){
        error("setsockopt(SO_REUSEADDR) failed");
    }
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    //func(sockfd); 
    //write(sockfd, "sc5:proj06:create", 17*sizeof(char));
    write(sockfd, "sc5:proj07:destroy", 18*sizeof(char));
    //write(sockfd, "abcdefghijklmnopqrstuvwxyz", 26*sizeof(char));
    // close the socket 
    close(sockfd); 
} 

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
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
        write(sockfd, buff, sizeof(buff)); 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        printf("From Server : %s", buff); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
    } 
} 
  
int main() 
{ 
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
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
    func(sockfd); 
  
    // close the socket 
    close(sockfd); 
} 

/*
int makeSocket(){
    int sfd = socket(<Address Family>, <transport/socket protocol>, <options>)
 	
    // get IP address/host name translated in to some useful format
    struct hostent* result = gethostbyname(<some addr or hostname>);
    
    // make the super-special struct that C requires for connect()
    struct sockaddr_in serverAddress;

    // zero it out
    bzero(&serverAddress, sizeof(serverAddress));

    // set the addr family (i.e. Internet)
    serverAddress.sin_family = AF_INET;

    // set port number to connect to ... also, convert my numeric type to a general 'network short' type
    serverAddress.sin_port = htons(7621);

    // copy the RAW BYTES! (ick!) from the hostent struct in to the sockaddr_in one
    bcopy( (char*)result->h_addr, (char*)&serverAddress.sin_addr.s_addr, result->addr_length   );
 
	connect(sfd, &serverAddress, sizeof(serverAddress)  );
}
*/
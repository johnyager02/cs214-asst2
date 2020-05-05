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
int parseInput(int, int, char**);

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
        printf("Configuration file set up. This IP address and Port will be used for future commands.\n");
        return 0;
    }
    if(compareString(command, "add") == 0){
        if(argc != 4){
            printf("Error: add takes Project Name and File Name as arguments");
            return 1;
        }
        add(argv[2], argv[3]);
    }
    if(compareString(command, "remove") == 0){
        if(argc != 4){
            printf("Error: remove takes Project Name and File Name as arguments");
            return 1;
        }
        removeEntry(argv[2], argv[3]);
    }
    else{
        //attempt to open Config file
        char* config = getFileContents("../.Config");

        sscanf(config, "%s %d", &IP, &PORT);
    }
    int connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    int sockfd = socket(AF_INET, SOCK_STREAM , 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(int)) < 0){
        error("setsockopt(SO_REUSEADDR) failed");
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
        perror("bind");
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    parseInput(sockfd, argc, argv);

    close(sockfd); 
} 


int parseInput(int sockfd, int argc, char** argv){
    char* command = argv[1];
    if(compareString(command, "checkout") == 0){
        if(argc != 3){
            printf("Error: checkout takes Project Name as only argument");
            return -1;
        }
        else{
            checkout(argv[2], sockfd);
        }
    }
    if(compareString(command, "update") == 0){
        if(argc != 3){
            printf("Error: update takes Project Name as only argument");
            return -1;
        }
        else{
            update(argv[2], sockfd);
        }
    }
    if(compareString(command, "upgrade") == 0){
        if(argc != 3){
            printf("Error: upgrade takes Project Name as only argument");
            return -1;
        }
        else{
            upgrade(argv[2], sockfd);
        }
    }
    if(compareString(command, "commit") == 0){
        printf("commit\n");
        if(argc != 3){
            printf("Error: commit takes Project Name as only argument");
            return -1;
        }
        else{
            commit(argv[2], sockfd);
        }
    }
    if(compareString(command, "push") == 0){
        if(argc != 3){
            printf("Error: push takes Project Name as only argument");
            return -1;
        }
        else{
            printf("pushing %s\n", argv[2]);
            push(argv[2], sockfd);
        }
    }
    if(compareString(command, "create") == 0){
        if(argc != 3){
            printf("Error: create takes Project Name as only argument");
            return -1;
        }
        else{
            create(argv[2], sockfd);
        }
    }
    if(compareString(command, "destroy") == 0){
        if(argc != 3){
            printf("Error: destroy takes Project Name as only argument");
            return -1;
        }
        else{
            destroy(argv[2], sockfd);
        }
    }
    if(compareString(command, "currentversion") == 0){
        if(argc != 3){
            printf("Error: currentversion takes Project Name as only argument");
            return -1;
        }
        else{
            currentversion(argv[2], sockfd);
        }
    }
    if(compareString(command, "history") == 0){
        if(argc != 3){
            printf("Error: history takes Project Name as only argument");
            return -1;
        }
        else{
            history(argv[2], sockfd);
        }
    }
    if(compareString(command, "rollback") == 0){
        if(argc != 4){
            printf("Error: history takes Project Name and version number as only arguments");
            return -1;
        }
        else{
            int versionNum;
            sscanf(argv[3], "%d", &versionNum);
            rollback(argv[2], versionNum, sockfd);
        }
    }
}
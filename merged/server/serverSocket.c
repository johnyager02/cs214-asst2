#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include "../stringFunc.h"
#include "../recursiveD.h"
#include "../sendAndReceive.h"
#include "../manifestFunc.h"
#define BUFFSIZE 10 
#define PORT 8080 
#define SA struct sockaddr 

int existsFile(char* filename){ 
    int file = open(filename, O_RDONLY, 00644);
    if(file < 0){
        return 0;
    }
    else{
        close(file);
        return 1;
    }
}

int existsDir(char* dirpath){
    DIR* dirptr = opendir(dirpath);
    if(dirptr){
        closedir(dirptr);
        return 1;
    } 
    else if(errno == ENOENT){
        return 0;
    }
}


void handleClientFetched(char** output, int clientSockFd){
    char* status = output[0];
    char* commandType = output[1];
    char* projName = output[2];
    char* fileName = output[3];
    
    if(existsDir(projName) != 1){
        //Failed to find requested project
        //send failed
        sendData(clientSockFd, projName, "");
    }
    if(existsFile(fileName) != 1){
        //Failed to find requested file
        //send failed
        sendData(clientSockFd, projName, "");
    }

    //Client fetched and file/project both exist -> send file that is requested
    sendData(clientSockFd, projName, fileName);

    //Free memory:

}

void handleClientSentCommand(char** output, int clientSockFd){
    char* status = output[0];
    char* commandType = output[1];
    char* projName = output[2];
    char* commandName = output[3];

    if(compareString(commandName, "push") == 0){
        /* The push command will fail if the project name doesn’t exist on the server.
        The client should send its .Commit and all files listed in it to the server. 
        The server should first lock the repository so no other command can be run on it. 
        While the repository is locked, the server should check to see if it has a stored .Commit for the client and that it is the same as the .Commit the client just sent. 
        If this is the case, the server should expire all other .Commits pending for any other clients, 
        duplicate the project directory, 
        write all the files the client sent to the newly-copied directory 
        (or remove files, as indicated in the .Commit), 
        update the new project directory's .Manifest by replacing corresponding entries for all files uploaded 
        (and removing entries for all files removed) with the information in the .Commit the client sent, 
        and increasing the project's version. 
        The server should then unlock the repository and send a success message to the client. 
        
        If there is a failure at any point in this process, the server should delete any new files or directories created, 
        unlock the repository and 
        send a failure message to the client. */
        printf("[handleClientSentCommand] Client sent command to push project: \"%s\"\n", projName);
    }
    else if(compareString(commandName, "create") == 0){
    /*The create command will fail if the project name already exists on the server 
    Otherwise, the server will create a project folder with the given name, initialize a .Manifest for it
    and send it to the client.*/
        printf("[handleClientSentCommand] Client sent command to create project: \"%s\"\n", projName);
        
        // Server: check if project exists:
        if(existsDir(projName) == 1){
            //send failure
            sendData(clientSockFd, projName, "");
        }

        //Create proj folder w/ name
        int makeDir = mkdir(projName, 0777);
        //Initialize .Manifest
        initializeManifest(projName);
        //Send new Manifest to client
        char* manifestPath = appendToStr(projName, "/.Manifest");
        char* filecontents = getFileContents(manifestPath);
        printf("[handleClientSentCommand] filecontents is: \"%s\"\n", filecontents);
        //sendData(clientSockFd, projName, filecontents);
        //free memory:
        // int i;
        // for(i=0;i<5;i++){
        //     free(output[i]);
        // }
        // free(output);
        // free(manifestPath);
    }
    else if(compareString(commandName, "destroy") == 0){
        /*The destroy command will fail if the project name doesn’t exist on the server
         On receiving a destroy command the server should lock the repository, expire any pending commits,
        delete all files and subdirectories under the project and send back a success message.*/

        //lock repository

        //expire pending commits

        //delete all files/subdirs under project

        //send back success
        printf("[handleClientSentCommand] Client sent command to destroy project: \"%s\"\n", projName);
    }
    else if(compareString(commandName, "currentversion") == 0){
        /*The currentversion command will request from the server the current state of a project from the server. This
        command does not require that the client has a copy of the project locally. The client should output a list of all
        files under the project name, along with their version number (i.e., number of updates).*/
        printf("[handleClientSentCommand] Client sent command to get currentversion of project: \"%s\"\n", projName);
    }
    else if(compareString(commandName, "history") == 0){
        /*The history command will fail if the project doesn’t exist on the server
        The server will send over a file containing the history of all operations performed on all successful pushes since the project's creation. The
        output should be similar to the update output, but with a version number and newline separating each push's log
        of changes.*/

        //send history
        printf("[handleClientSentCommand] Client sent command to send history of project: \"%s\"\n", projName);
    }
    else if(compareString(commandName, "rollback")){
        /*The rollback command will fail if the project name doesn’t exist on the server or the version number given is invalid. 
         The server will revert its current version of the project back to the version number requested by
        the client by deleting all more recent versions saved on the server side.*/

        //delete more recent versions past the requested version on serverside

        printf("[handleClientSentCommand] Client sent command to rollback the project: \"%s\"\n", projName);
    }
}

char** readInputFromClient(char* buff, int fd){
    char success = buff[0];
    if(success == 'f'){
        printf("[readInput] Error! command could not be executed\n");
        return NULL;
    }
    char commandType = buff[1];
    char* projectLengthString = mallocStr(5);
    int n = 2;
    while(buff[n++] != ':'){
        projectLengthString[n-3] = buff[n-1];
        if((strlen(projectLengthString) % 5) == 0){
            projectLengthString = reallocStr(projectLengthString, 5 + strlen(projectLengthString));
        }
    }
    int projectLength = atoi(projectLengthString);
    char* projectName = mallocStr(projectLength+1);
    strncpy(projectName, buff+n, projectLength);
    projectName[projectLength] = '\0';
    n+= projectLength;
    //free(projectLengthString);
    char* fileLengthString = mallocStr(5);
    int a = n+1;
    while(buff[n++] != ':'){
        fileLengthString[n-a] = buff[n-1];
        if((strlen(fileLengthString) % 5) == 0){
            fileLengthString = reallocStr(fileLengthString, 5 + strlen(fileLengthString));
        }
    }
    int fileLength = atoi(fileLengthString);
    char* fileName = mallocStr(fileLength+1);
    strncpy(fileName, buff+n, fileLength);
    fileName[fileLength] = '\0';
    //free(fileLengthString);
    n+=fileLength;

    if(commandType == 's'){
        a = n+1;
        char* dataLengthString = mallocStr(5);
        printf("%s\n", buff+n);
        while(buff[n++] != ':'){
            dataLengthString[n-a] = buff[n-1];
            if((strlen(dataLengthString) % 5) == 0){
                dataLengthString = reallocStr(dataLengthString, 5 + strlen(dataLengthString));
            }
        }
        int dataLength = atoi(fileLengthString);
        char* data = mallocStr(dataLength+1);
        strncpy(data, buff+n, dataLength);
        data[dataLength] = '\0';
        //free(fileLengthString);
        printf("[readInput] %c %c %s %s %d %s\n", success, commandType, projectName, fileName, dataLength, data);

        //handleSend
        return (char**) getOutputArrSent(success, commandType, projectName, fileName, data);
    }
    else if(commandType == 'f'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleFetch
        handleClientFetched((char**) getOutputArrFetched(success, commandType, projectName, fileName), fd);
        return NULL;
    }
    else if(commandType == 'c'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleSendCommand
        handleClientSentCommand( (char**) getOutputArrFetched(success, commandType, projectName, fileName), fd );
        return NULL;
    }
    else{
        printf("Error: command type not recognized");
        return NULL;
    }
}

// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
    char* buff = (char*) mallocStr(BUFFSIZE+1);
    bzero(buff, (BUFFSIZE+1)*sizeof(char)); 
    int n; 
    int numBytesRead = 0;
    int totalReadInBytes = 0;
    int currentBufferSize = BUFFSIZE;
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, currentBufferSize + 1); 
        // read the message from client and copy it in buffer
        printf("[func] Reading message now: \n");
        while((numBytesRead = read(sockfd, buff + totalReadInBytes, 5*sizeof(char))) != 0){
            
            printf("[func] Current buffer is: \"%s\"\n", buff);
            totalReadInBytes+=numBytesRead;
            printf("[func] NumBytesRead is: %d\n", numBytesRead);
            if(totalReadInBytes==currentBufferSize){//realloc buff
                printf("[func] Reallocing buffer\n");
                buff = (char*) reallocStr(buff, 2*currentBufferSize + 1);
                currentBufferSize = 2*currentBufferSize;
                printf("[func] Current buffer size is: %d\n", currentBufferSize);
                memset(buff + totalReadInBytes, '\0', (currentBufferSize + 1 - totalReadInBytes)*sizeof(char));
            }
            printf("[func] totalReadInBytes is: %d\n", totalReadInBytes);
        }
        if(strlen(buff) != 0){ // done reading
            printf("[func] final buffer after read is: \"%s\"\n", buff);
            readInputFromClient(buff, sockfd);
        }
        // print buffer which contains the client contents 
        //printf("From client: %s\t To client : ", buff); 
        printf("[func] Done reading input\n");
        bzero(buff, currentBufferSize); 
        totalReadInBytes = 0;  
        numBytesRead = 0;
        currentBufferSize = BUFFSIZE;
        buff = (char*) reallocStr( buff, currentBufferSize + 1);
        printf("[func] Current buffer size is: %d\n", currentBufferSize);
        n = 0; 
        // copy server message in the buffer 
        // while ((buff[n++] = getchar()) != '\n') 
        //     ; 
  
        // and send that buffer to client 
        //write(sockfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 

// Driver function 
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0){
        error("setsockopt(SO_REUSEADDR) failed");
    }
    printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
  
    // Function for chatting between client and server 
    func(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
} 
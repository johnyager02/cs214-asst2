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
#include<signal.h>
#include "../manifestFunc.h"
#include<pthread.h>
#define BUFFSIZE 10 
#define SA struct sockaddr 
pthread_mutex_t lock;
typedef struct mutexHolder{
    pthread_mutex_t lock;
    char* project;
    struct mutexHolder* next;
}mutexHolder;

mutexHolder* mutexes = NULL;


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
        sendData(clientSockFd, projName, manifestPath);
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

        printf("[handleClientSentCommand] Client sent command to destroy project: \"%s\"\n", projName);
        //lock repository

        //expire pending commits

        //delete all files/subdirs under project
        int projLen = strlen(projName);
        if(projName[projLen - 1] != '/'){
            char* oldStr = projName;
            projName = appendToStr(projName, "/");
            free(oldStr);
        }
        recursiveDelete(projName);
        if(isEmptyDir(projName)){
            printf("[handleClientSentCommand] Deleted proj: \"%s\"\n", projName);
            //Delete empty proj dir
            rmdir(projName);
            //send back success
            sendData(clientSockFd, projName, "success");
            
        } else{
            printf("[handleClientSentCommand] Failed to delete proj: \"%s\"\n", projName);
            //send back failed
            sendData(clientSockFd, projName, "");
        } 
        
        
    }
    else if(compareString(commandName, "currentversion") == 0){
        /*The currentversion command will request from the server the current state of a project from the server. This
        command does not require that the client has a copy of the project locally. The client should output a list of all
        files under the project name, along with their version number (i.e., number of updates).*/
        printf("[handleClientSentCommand] Client sent command to get currentversion of project: \"%s\"\n", projName);
        // Server: check if project exists:
        if(existsDir(projName) == 1){
            //send failure
            sendData(clientSockFd, projName, "");
        }
        //Send  Manifest to client
        char* manifestPath = appendToStr(projName, "/.Manifest");
        char* filecontents = getFileContents(manifestPath);
        printf("[handleClientSentCommand] filecontents is: \"%s\"\n", filecontents);
        sendData(clientSockFd, projName, manifestPath);
    }
    else if(compareString(commandName, "history") == 0){
        /*The history command will fail if the project doesn’t exist on the server
        The server will send over a file containing the history of all operations performed on all successful pushes since the project's creation. The
        output should be similar to the update output, but with a version number and newline separating each push's log
        of changes.*/

        //send history
        printf("[handleClientSentCommand] Client sent command to send history of project: \"%s\"\n", projName);
    }
    else if(compareString(commandName, "rollback") == 0){
        /*The rollback command will fail if the project name doesn’t exist on the server or the version number given is invalid. 
         The server will revert its current version of the project back to the version number requested by
        the client by deleting all more recent versions saved on the server side.*/

        //delete more recent versions past the requested version on serverside

        printf("[handleClientSentCommand] Client sent command to rollback the project: \"%s\"\n", projName);
    }
}

void addMutexToList(mutexHolder* m){
    if(mutexes == NULL){
        mutexes = m;
    }
    else{
        m->next = mutexes;
        mutexes = m;
    }
    return;
}

pthread_mutex_t findMutex(char* proj){
    mutexHolder* ptr = mutexes;
    while(ptr != NULL){
        if(compareString(proj, ptr->project) == 0){
            return ptr->lock;
        }
        ptr = ptr->next;
    }
    printf("error: mutex for project %s could not be found\n", proj);
    exit(1);
}

int createMutexes(){
    DIR* currentDirPtr = opendir("./");
    if(currentDirPtr == NULL){
        printf("DIR not found!\n");
        return -1;
    }
    struct dirent* currentPtr;
    readdir(currentDirPtr);
    readdir(currentDirPtr);
    currentPtr = readdir(currentDirPtr);
    while(currentPtr!=NULL){

        if(currentPtr->d_type == DT_DIR){
            char* dirName = currentPtr->d_name;
            if(compareString(dirName, "previous") != 0){
                mutexHolder* newMutex = malloc(sizeof(mutexHolder));
                newMutex->project = dirName;
                if(pthread_mutex_init((&newMutex->lock), NULL) != 0){
                    printf("Error creating mutex for %s\n", dirName);
                    exit(1);
                }
                newMutex->next = NULL;
                addMutexToList(newMutex);
            }
        }
        currentPtr = readdir(currentDirPtr);
    }
    close(currentDirPtr);
    return 0;
}


void destroyMutexes(){
    mutexHolder* ptr = mutexes;
    while(ptr != NULL){
        pthread_mutex_destroy(&(ptr->lock));
        mutexHolder* f = ptr;
        ptr = ptr->next;
        free(f->project);
        free(f);
    }
    return;
}


char** readInputFromClient(int sockfd){
    //Want to read <s/f><s/f/c><projLen>:<projName><fileLen>:<fileName> or for send:<s/f><s/f/c><projLen>:<projName><fileLen>:<fileName><dataLen>:<data>
    //Read successflag and commandType
    printf("Start read\n");
    char* firstRead = readFromSock(sockfd, 2*sizeof(char));
    printf("firstRead %s\n", firstRead);
    if(firstRead==NULL){
        //Failed
        printf("[readInputProtocol] Error: firstRead NULL\n");
        return NULL;
    }
    if(strlen(firstRead) != 2){
        //Failed
        printf("[readInputProtocol] Error: firstRead did not read 2 bytes\n");
        return NULL;
    }
    //Handle success or fail
    char success = firstRead[0];
    if(success != 'f' && success != 's'){
        printf("[readInputProtocol] Unknown message\n");
        return NULL;
    }
    if(success == 'f'){
        printf("[readInputProtocol] Error! command could not be executed\n");
        return NULL;
    }
    //Parse commandType
    char commandType = firstRead[1];

    //Read project length
    char* projectLengthString = getNextUnknownLen(sockfd);
    
    int projectLength;
    sscanf(projectLengthString, "%d", &projectLength);
    
    //Read projName
    char* projectName = mallocStr(projectLength+1);
    bzero(projectName, (projectLength+1)*sizeof(char));
    projectName = readFromSockIntoBuff(sockfd, projectName, projectLength);

    //Read fileName Length:
    char* fileLengthString = getNextUnknownLen(sockfd);
    int fileLength;
    sscanf(fileLengthString, "%d", &fileLength);
    
    //Read fileName:
    char* fileName = mallocStr(fileLength+1);
    bzero(fileName, (fileLength+1)*sizeof(char));
    fileName = readFromSockIntoBuff(sockfd, fileName, fileLength);

    //Done reading...
    
    pthread_mutex_t mu = findMutex(projectName);
    pthread_mutex_lock(&mu);
    printf("Mutex %s locked\n", projectName);

    if(commandType == 's'){
        //Read dataLen:
        char* dataLengthString = getNextUnknownLen(sockfd);
        int dataLength;
        sscanf(dataLengthString, "%d", &dataLength);
        
        //Read data:
        char* data = mallocStr(dataLength+1);
        bzero(data, (dataLength+1)*sizeof(char));
        data = readFromSockIntoBuff(sockfd, data, dataLength);

        printf("[readInputProtocol] %c %c %s %s %d %s\n", success, commandType, projectName, fileName, dataLength, data);
        char** temp = malloc(sizeof(char*)*2);
        temp[0] = "st";
        temp[1] = "ff";
        pthread_mutex_unlock(&mu);
        printf("Mutex %s unlocked\n", projectName);
        return temp;
        //handleSend
        //return (char**) getOutputArrSent(success, commandType, projectName, fileName, data);
    }
    else if(commandType == 'f'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleFetch
        char** output = (char**) getOutputArrFetched(success, commandType, projectName, fileName);
        handleClientFetched( output, sockfd);
        pthread_mutex_unlock(&mu);
        printf("Mutex %s unlocked\n", projectName);
        return output;
    }
    else if(commandType == 'c'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleSendCommand
        if(compareString(fileName, "stop") == 0 ){
            return NULL;
        }
        handleClientSentCommand( (char**) getOutputArrFetched(success, commandType, projectName, fileName), sockfd);
        return NULL;
    }
    else{
        printf("Error: command type not recognized");
        pthread_mutex_unlock(&mu);

        return NULL;
    }
    

    return NULL;
}

void* threadMaker(void* arg){
    int sockfd = *(int*)arg;
    printf("[threadmaker: Starting thread %d\n", sockfd);
    while(readInputFromClient(sockfd) != NULL){     
    }
    printf("[threadmaker]: Finished thread %d\n", sockfd);
    return NULL;
}

void testfunc(int sockfd){
    struct sockaddr_in cli; 
    int len, connfd;
    while(1){
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
        
        pthread_t clientThread;
        int* fd = malloc(sizeof(int));
        *fd = connfd;
        int e = pthread_create(&clientThread, NULL, threadMaker, (void*)fd);
        if(e != 0){
            perror("Create thread");
            return;
        }
        pthread_join(clientThread, NULL);
        pthread_mutex_destroy(&lock);
    }
    close(connfd);
    return;
}



// Driver function 
int main(int argc, char** argv) { 
    int PORT;
    if(argc != 2){
        printf("must provide port number as argument\n");
        return 1;
    }
    sscanf(argv[1], "%d", &PORT);
    int sockfd;
    struct sockaddr_in servaddr;
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(int)) < 0){
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
        perror("bind");
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    
  
    // Function for chatting between client and server 
    //readInputFromClient(connfd); 
    createMutexes();
    testfunc(sockfd);
    
    // After chatting close the socket 
    close(sockfd); 
    destroyMutexes();
} 
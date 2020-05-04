#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include<openssl/sha.h>
#include"stringFunc.h"
#include"recursiveD.h"
#include"manifestFunc.h"
#include"sendAndReceive.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFFSIZE 10
void handleServerFetched(char** output, int clientSockFd){
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

char** readInputFromServer(int sockfd){
    //Want to read <s/f><s/f/c><projLen>:<projName><fileLen>:<fileName> or for send:<s/f><s/f/c><projLen>:<projName><fileLen>:<fileName><dataLen>:<data>
    //Read successflag and commandType
    char* firstRead = readFromSock(sockfd, 2*sizeof(char));
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
    printf("[readInputProtocol] projLenStr is: \"%s\"\n", projectLengthString);
    
    int projectLength;
    sscanf(projectLengthString, "%d", &projectLength);
    
    //Read projName
    char* projectName = mallocStr(projectLength+1);
    bzero(projectName, (projectLength+1)*sizeof(char));
    projectName = readFromSockIntoBuff(sockfd, projectName, projectLength);
    printf("[readInputProtocol] projName is: \"%s\"\n", projectName);

    //Read fileName Length:
    char* fileLengthString = getNextUnknownLen(sockfd);
    printf("[readInputProtocol] fileLenStr is: \"%s\"\n", fileLengthString);
    int fileLength;
    sscanf(fileLengthString, "%d", &fileLength);
    
    //Read fileName:
    char* fileName = mallocStr(fileLength+1);
    bzero(fileName, (fileLength+1)*sizeof(char));
    fileName = readFromSockIntoBuff(sockfd, fileName, fileLength);
    printf("[readInputProtocol] fileName is: \"%s\"\n", fileName);

    //Done reading...
    printf("[readInputProtocol] Done Reading -> handling commandType cases\n");
    
    
    if(commandType == 's'){
        //Read dataLen:
        char* dataLengthString = getNextUnknownLen(sockfd);
        printf("[readInputProtocol] DataLen is: \"%s\"\n", dataLengthString);
        int dataLength;
        sscanf(dataLengthString, "%d", &dataLength);
        
        //Read data:
        char* data = mallocStr(dataLength+1);
        bzero(data, (dataLength+1)*sizeof(char));
        data = readFromSockIntoBuff(sockfd, data, dataLength);

        printf("[readInputProtocol] %c %c %s %s %d %s\n", success, commandType, projectName, fileName, dataLength, data);

        //handleSend
        //return (char**) getOutputArrSent(success, commandType, projectName, fileName, data);
    }
    else if(commandType == 'f'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleFetch
        //return  (char**) getOutputArrFetched(success, commandType, projectName, fileName);
    }
    else if(commandType == 'c'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleSendCommand
        //return (char**) getOutputArrFetched(success, commandType, projectName, fileName);
    }
    else{
        printf("Error: command type not recognized");
        return NULL;
    }
    return NULL;
}
//NOTE! format of names in arguments for filenams/dirnames: "file1" || "sub0/subsub92/file716"

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

char* filepathToName(char* filepath){ //given filepath -> return filename EX) "./proj0/test0" -> "test0"
    char* filename = (char*) mallocStr(strlen(filepath));
    memset(filename, '\0', strlen(filepath)*sizeof(char));

    //
    int i;
    int indexOfCopy = 0;
    for(i = 0; i<strlen(filepath);i++){
        if(filepath[i] == '/'){//reset buffer
            memset(filename, '\0', strlen(filepath)*sizeof(char));
            indexOfCopy = 0;
        }
        else{
            memcpy(filename + indexOfCopy, filepath + i, 1*sizeof(char));
            indexOfCopy++;
        }
    }
    return filename;
}

char* nthDirPathToName(char* dirpath, int n){ //given dirpath -> return nth dirname EX) "./proj0/dir0/" w/ n=2-> "dir0"; n is like array index, n=0 is first dirname "."
    char* dirname = (char*) mallocStr(strlen(dirpath));
    memset(dirname, '\0', strlen(dirpath)*sizeof(char));

    //
    int i;
    int indexOfCopy = 0;
    int numSlash = 0;
    for(i = 0; i<strlen(dirpath);i++){
        if(dirpath[i] == '/'){//reset buffer
            if(numSlash == n){
                return dirname;
            }
            memset(dirname, '\0', strlen(dirpath)*sizeof(char));
            indexOfCopy = 0;
            numSlash++;
        }
        else{
            memcpy(dirname + indexOfCopy, dirpath + i, 1*sizeof(char));
            indexOfCopy++;
        }
    }
    return NULL;
}


void configure(char* hostname, char* port){
    //create ./configure file that contains IP and port 
}

void checkout(char* projname, int sockfd){

}

void update(char* projname, int sockfd){

}

void upgrade(char* projname, int sockfd){

}

void commit(char* projname, int sockfd){
    //START SERVERSIDE:
    if(existsFile(projname) == 0){
        //fail
    }

    //START CLIENTSIDE:
    //fetch server's manifest:
    int projectNameLenInt = strlen(projname);
    char* projectNameLen = numToStr(projectNameLenInt);
    
    char* manifestPath = appendToStr(projname, "/.Manifest");
    int fileNameLenInt = strlen(manifestPath);
    char* fileNameLen = numToStr(fileNameLenInt);
    
    int totalBufferSize = 2 + strlen(projectNameLen) + 1 + strlen(projname) + strlen(fileNameLen) + 1 + strlen(manifestPath);
    char* fetchClientToServerCommit = (char*) mallocStr(totalBufferSize + 1);
    memset(fetchClientToServerCommit, '\0', (totalBufferSize+1)*sizeof(char));
                                                            //<s><f><projectNameLength>:<projectName><fileNameLength>:<fileName>
    sprintf(fetchClientToServerCommit, "%c%c%s:%s%s:%s", 's', 'f', projectNameLen, projname, fileNameLen, manifestPath);

    

    char* conflictPath = appendToStr(projname, "/.Conflict");
    if(existsFile(conflictPath) == 1){

    }

    free(conflictPath);
    return;
}

void push(char* projname, int sockfd){
    //char buff[10];
    //sendData(sockfd, "myProject", "filename");
    printf("pushing\n");
    
    //char* str = "sc5:proj18:testfile";
    //char* str = "Hello";
    char* str = "ss5:proj111:proj1/test120:test1test1test1test1"; //46 bytes
    writeToSock(sockfd, str);
    
    //readInputProtocol(sockfd);
}

void create(char* projname, int sockfd){
    //1) Client sendCommand -> "create" 
    sendCommand(sockfd, projname, "create");

    //START CLIENTSIDE: receiving message
    //Client: Receives sendServerToClientCreate string and then parses...-> 
    char* buff = (char*) mallocStr(BUFFSIZE+1);
    bzero(buff, (BUFFSIZE+1)*sizeof(char)); 
    int n; 
    int numBytesRead = 0;
    int totalReadInBytes = 0;
    int currentBufferSize = BUFFSIZE;
    char** output;
    while((numBytesRead = recv(sockfd, buff + totalReadInBytes, 5*sizeof(char), MSG_DONTWAIT)) != 0){
            if(numBytesRead>0){
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
        }
        if(strlen(buff) != 0){ // done reading
        printf("[func] final buffer after read is: \"%s\"\n", buff);
        output = readInputFromServer(sockfd);
    }
    if(output == NULL){
        printf("[create] Failed!\n");
        close(sockfd);
        return;
    }
    if(output[0][0] == 'f'){
        printf("[create] Failed!\n");
        close(sockfd);
        return;
    }
    char* filedata = output[4];
    int makeDir = mkdir(projname, 0777);
    char* manifestPath = appendToStr(projname, "/.Manifest");
    int manifestFileClient = open(manifestPath, O_RDWR | O_CREAT, 00644);
    int numBytesToWrite = strlen(filedata);
    int numBytesWritten = 0;
    int totalNumBytesWritten = 0;
    while(numBytesToWrite > 0){
        numBytesWritten = write(manifestFileClient, filedata + totalNumBytesWritten, (numBytesToWrite)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
        totalNumBytesWritten+=numBytesWritten;
    }
    close(manifestFileClient);
    close(sockfd);
    if(totalNumBytesWritten == strlen(filedata)){
        printf("[create] Success!\n");
    }
    // free(filedata);
    // free(manifestPath);
}

void destroy(char* projname, int sockfd){

}

void add(char* projname, char* filename){ // Expects projname as format: "proj0" and filename format as: "test0" && "proj0/test0"
    if(existsDir(projname) != 1){ //Checks if project exists
        //Failed
        printf("[add] ERROR! This proj: \"%s\" does not exist in the client's machine\n", projname);
        return;
    }
    char* manifestPath = appendToStr(projname, "/.Manifest");
    if(getFileLineManifest(manifestPath, filename, "-ps")!=NULL){//file already in manifest
        printf("[add] ERROR! This file: \"%s\" already exists in the Manifest\n", filename);
        return;
    } 
    //File exists and is not already in manifest -> append to .Manifest
    printf("[add] Adding file: \"%s\" to the manifest of project: \"%s\"\n", filename, projname);
    char* lineToAdd = getLineToAdd(0, filename);
    addToManifest(manifestPath, lineToAdd);
}

void removeEntry(char* projname, char* filename){ // Expects projname as format: "proj0" and filename format as: "test0" && "proj0/test0"
    if(existsDir(projname) != 1){ //Checks if project exists
        //Failed
        printf("[removeEntry] ERROR! This proj: \"%s\" does not exist in the client's machine\n", projname);
        return;
    }
    char* manifestPath = appendToStr(projname, "/.Manifest");
    char* lineNumStr = getFileLineManifest(manifestPath, filename, "-pi");
    int lineNumToDelete;
    if(lineNumStr == NULL){
        lineNumToDelete = -1;
    }
    else{
        lineNumToDelete;
        sscanf(lineNumStr, "%d", &lineNumToDelete);
    }
    if(lineNumToDelete == -1){//file to be deleted is NOT in manifest
        printf("[removeEntry] ERROR! This file: \"%s\" does NOT exist in the Manifest\n", filename);
        return;
    } 
    //File exists and is in the manifest -> delete entry
    printf("[removeEntry] Deleting file: \"%s\" from the manifest of project: \"%s\"\n", filename, projname);
    removeLine(manifestPath, lineNumToDelete);
}

void currentversion(char* projname, int sockfd){
    int projNameLen = strlen(projname);
    char* manifestPath = appendToStr(projname, "/.Manifest");
    char currentVersion[] = "currentVersion";
    int commandLen = strlen(currentVersion);
    //Client -> Server: sendCommand:  "<s><c><projNameLen>:<projname><commandLen>:<currentVersion>"


    //Server -> Client: sendCommand: "<s><c><projNameLen>:<projname><dataLen>:<filenames:versions>" ... <filenames:versions> is the data internally separated by ':'
    //EX) "<s><c><5>:<proj0><46>:<Makefile:0:proj0/test0:1:proj0/sub0/subtest0:3>" gets sent from server to client
    char* receivedStr = "Makefile:0:proj0/test0:1:proj0/sub0/subtest0:3:proj0/test1:35"; // replace hardcoded with what server sends client
    
    //Parse receivedStr for filenames and their versions to print
    char* fileToPrint = (char*) mallocStr(strlen(receivedStr));
    memset(fileToPrint, '\0', strlen(receivedStr)*sizeof(char));
    char* versionToPrint = (char*) mallocStr(strlen(receivedStr));
    memset(versionToPrint, '\0', strlen(receivedStr)*sizeof(char));

    // EX string to parse: "Makefile:0:proj0/test0:1:proj0/sub0/subtest0:3:proj0/test1:35" -> prints w/ \t after filename and \n after version for visual format
    int i;
    int indexOfCopy = 0;
    int numDelims = 0;
    int copyIntoFile = 1;
    for(i = 0; i<strlen(receivedStr);i++){
        if(receivedStr[i] == ':'){//reset buffer
            if(numDelims % 2 == 0){ // even -> file
                printf("%s\t", fileToPrint);
                memset(fileToPrint, '\0', strlen(receivedStr)*sizeof(char));
                copyIntoFile = 0;
            }
            else if(numDelims % 2 == 1){// odd -> version
                printf("%s\n", versionToPrint);
                memset(versionToPrint, '\0', strlen(receivedStr)*sizeof(char));
                copyIntoFile = 1;
            }
            indexOfCopy = 0;
            numDelims++;
        }
        else if(i+1 == strlen(receivedStr)){//last char
            memcpy(versionToPrint + indexOfCopy, receivedStr+i, 1*sizeof(char));
            printf("%s\n", versionToPrint);
        }
        else{
            if(copyIntoFile == 1){
                memcpy(fileToPrint + indexOfCopy, receivedStr + i, 1*sizeof(char));
            }
            else if(copyIntoFile == 0){
                memcpy(versionToPrint + indexOfCopy, receivedStr + i, 1*sizeof(char));
            }
            indexOfCopy++;
        }
    }
}

void history(char* projname, int sockfd){

}

void rollback(char* projname, int version, int sockfd){
    
}
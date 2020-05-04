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

char** readInputFromServer(char* buff, int fd){
    printf("%s\n", buff);
    char success = buff[0];
    if(success != 'f' || success != 's'){
        return;
    }
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
        handleServerFetched((char**) getOutputArrFetched(success, commandType, projectName, fileName), fd);
        return NULL;
    }
    else if(commandType == 'c'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleSendCommand
        //handleServerSentCommand( (char**) getOutputArrFetched(success, commandType, projectName, fileName), fd );
        return NULL;
    }
    else{
        printf("Error: command type not recognized");
        return NULL;
    }
}

//NOTE! format of names in arguments for filenames/dirnames: "file1" || "sub0/subsub92/file716"


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
/*If it does run it will request the entire project from the server, which will send over the current version of the
project .Manifest as well as all the files that are listed in it. The client will be responsible for receiving the
project, creating any subdirectories under the project and putting all files in to place as well as saving the
.Manifest. */
   
    char* manifestPath = appendToStr(projname, "/.Manifest");
    
    
    //fetchData(sockfd, projname, manifestPath);
    //receive manifest into char** output output[0] == status, output[1] == commandType, output[2] == projName, output[3] = fileName, output[4] = filedata; 
    //char* filedata = output[4];
    
    //checks if project exists -> if not make
    if(existsDir(projname) != 1){
        int mkProj = mkdir(projname, 0777);
    }
    //create manifest
    initializeManifest(projname);
    //setFileContents to manifest received from server;
    
    //TESTING DATA: -> Comment out after getting server receive to work!
    char* testServerManifestData = getFileContents("server/proj1/.Manifest");
    char* testServerFile1 = getFileContents("server/proj1/test0");
    char* testServerFile2 = getFileContents("server/proj1/test1");
    char* testServerFile3 = getFileContents("server/proj1/test2");
    
    overwriteOrCreateFile(manifestPath, testServerManifestData);

    //write all files to their locations
    int numLinesManifest = getNumLines(manifestPath);
    
    int i;
    for(i = 1; i<numLinesManifest;i++){
        char* fetchLine = getLineFile(manifestPath, i);
        char* fetchFileName = nthToken(fetchLine, 1, ' ');
        printf("[checkout] Filename of line %d to fetch is: \"%s\"\n", i, fetchFileName); 
        //fetch file from server
        //fetchData(sockfd, projname, fetchFileName);
    }
    createNewFile("proj1/test0");
    createNewFile("proj1/test1");
    createNewFile("proj1/test2");
    overwriteOrCreateFile("proj1/test0", testServerFile1);
    overwriteOrCreateFile("proj1/test1", testServerFile2);
    overwriteOrCreateFile("proj1/test2", testServerFile3);
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
        output = readInputFromServer(buff, sockfd);
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
    sendCommand(sockfd, projname, "destroy");
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
        lineNumToDelete = atoi(lineNumStr);
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
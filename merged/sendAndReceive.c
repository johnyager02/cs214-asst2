#include<stdlib.h>
#include<stdio.h>
#include"stringFunc.h"
#include"manifestFunc.h"
#include<string.h>

char** getOutputArrSent(char status, char commandType, char* projectName, char* fileName, char* data){
    
    char* statusStr = (char*) mallocStr(2);
    memset(statusStr, status, 1*sizeof(char));
    memset(statusStr + 1, '\0', 1*sizeof(char));

    char* commandTypeStr = (char*) mallocStr(2);
    memset(commandTypeStr, commandType, 1*sizeof(char));
    memset(commandTypeStr + 1, '\0', 1*sizeof(char));

    char** output = (char**) malloc(5*sizeof(*output));
    output[0] = statusStr;
    output[1] = commandTypeStr;
    output[2] = projectName;
    output[3] = fileName;
    output[4] = data;
    return output;
}

char** getOutputArrFetched(char status, char commandType, char* projectName, char* fileName){
    
    char* statusStr = (char*) mallocStr(2);
    memset(statusStr, status, 1*sizeof(char));
    memset(statusStr + 1, '\0', 1*sizeof(char));

    char* commandTypeStr = (char*) mallocStr(2);
    memset(commandTypeStr, commandType, 1*sizeof(char));
    memset(commandTypeStr + 1, '\0', 1*sizeof(char));

    char** output = (char**) malloc(4*sizeof(*output));
    output[0] = statusStr;
    output[1] = commandTypeStr;
    output[2] = projectName;
    output[3] = fileName;
    return output;
}

char* readFromSock(int sockfd, int numBytesToRead){
    char* buff = (char*) mallocStr(numBytesToRead+1);
    bzero(buff, (numBytesToRead+1)*sizeof(char)); 
    int numBytesRead = 0;
    int totalReadInBytes = 0;
    int currentBufferSize = numBytesToRead;
    int readFinished = 0;
    do{
        //Read until read enough bytes in total as specified by numBytesToRead...
        numBytesRead = read(sockfd, buff + totalReadInBytes, (numBytesToRead-totalReadInBytes)*sizeof(char));
        //Read enough ->stop reading
        if(totalReadInBytes == numBytesToRead){
            readFinished = 1;
            break;
        }
        //Actually read into buffer >0 bytes
        if(numBytesRead>0){
            printf("[readFromSock] Current buffer is: \"%s\"\n", buff);
            totalReadInBytes+=numBytesRead;
            printf("[readFromSock] NumBytesRead is: %d\n", numBytesRead);
            if(totalReadInBytes==currentBufferSize){//realloc buff
                printf("[readFromSock] Reallocing buffer\n");
                buff = (char*) reallocStr(buff, 2*currentBufferSize + 1);
                currentBufferSize = 2*currentBufferSize;
                printf("[readFromSock] Current buffer size is: %d\n", currentBufferSize);
                memset(buff + totalReadInBytes, '\0', (currentBufferSize + 1 - totalReadInBytes)*sizeof(char));
            }
            printf("[readFromSock] totalReadInBytes is: %d\n", totalReadInBytes);
        }
    }while(readFinished ==0);
    if(strlen(buff) != 0){ // done reading
        printf("[readFromSock] final buffer after read is: \"%s\"\n", buff);
    }
    return buff;
}

char* readFromSockIntoBuff(int sockfd, char* buff, int numBytesToRead){ 
    int numBytesRead = 0;
    int totalReadInBytes = 0;
    int currentBufferSize = numBytesToRead;
    printf("[readFromSockIntoBuff] numBytesToRead is: %d\n", numBytesToRead);
    int readFinished = 0;
    do{
        //Read until read enough bytes in total as specified by numBytesToRead...
        numBytesRead = read(sockfd, buff + totalReadInBytes, (numBytesToRead-totalReadInBytes)*sizeof(char));
        //Read enough ->stop reading
        //Actually read into buffer >0 bytes
        if(numBytesRead>0){
            //printf("[readFromSockIntoBuff] Current buffer is: \"%s\"\n", buff);
            totalReadInBytes+=numBytesRead;
            if(totalReadInBytes == numBytesToRead){
                readFinished = 1;
                break;
            }
            //printf("[readFromSockIntoBuff] NumBytesRead is: %d\n", numBytesRead);
            if(totalReadInBytes==currentBufferSize){//realloc buff
                printf("[readFromSockIntoBuff] Reallocing buffer\n");
                buff = (char*) reallocStr(buff, 2*currentBufferSize + 1);
                currentBufferSize = 2*currentBufferSize;
                printf("[readFromSockIntoBuff] Current buffer size is: %d\n", currentBufferSize);
                memset(buff + totalReadInBytes, '\0', (currentBufferSize + 1 - totalReadInBytes)*sizeof(char));
            }
            printf("[readFromSockIntoBuff] totalReadInBytes is: %d\n", totalReadInBytes);
        }
    }while(readFinished ==0);
    if(strlen(buff) != 0){ // done reading
        printf("[readFromSockIntoBuff] final buffer after read is: \"%s\" w/ total bytes: %d\n", buff, totalReadInBytes);
    }
    return buff;
}

int writeToSock(int sockfd, char* buff){
    int numBytesToWrite = strlen(buff);
    int numBytesWritten = 0;
    int totalNumBytesWritten = 0;
    printf("[writeToSock] Writing buffer: \"%s\"\n", buff);
    printf("[writeToSock] str len of buffer is: %d\n", numBytesToWrite);
    do{
        numBytesWritten = write(sockfd, buff + totalNumBytesWritten, (numBytesToWrite-totalNumBytesWritten)*sizeof(char));
        printf("Numbyteswritten is: %d\n", numBytesWritten);
        if(numBytesWritten > 0){
            numBytesToWrite-=numBytesWritten;
            totalNumBytesWritten+=numBytesWritten;
        }
    }while(numBytesToWrite>0 && buff[totalNumBytesWritten]!='\0');
    if(totalNumBytesWritten == numBytesToWrite){
        return 1;
    }
    return 0;
}

void sendData(int sockfd, char* projectName, char* fileName){
    int projectLengthInt = strlen(projectName);
    char* projectLengthString = numToStr(projectLengthInt);
    int fileLengthInt = strlen(fileName);
    char* fileLengthString = numToStr(fileLengthInt);
    char* data;
    if(fileLengthInt>0){
        if(existsFile(fileName) == 1){ //is file -> sending over filedata
            data = getFileContents(fileName);
        }
        else{// message
            data = fileName;
        }
    }else{
        data = "";
    }
    //char* data = "file contents are right here";
    int dataLengthInt = strlen(data);
    char* dataLengthString = numToStr(dataLengthInt);
    char success = (dataLengthInt < 1) ? 'f' : 's';
    char* buff = mallocStr(2 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + strlen(dataLengthString) + 1 + dataLengthInt + 1);
    sprintf(buff, "%c%c%s:%s%s:%s%s:%s", success, 's', projectLengthString, projectName, fileLengthString, fileName, dataLengthString, data);
    buff[2 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + strlen(dataLengthString) + 1 + dataLengthInt] = '\0';
    printf("[sendData] \"%s\"\n", buff);
    writeToSock(sockfd, buff);
}

void fetchData(int sockfd, char* projectName, char* fileName){
    int projectLengthInt = strlen(projectName);
    char* projectLengthString = numToStr(projectLengthInt);
    int fileLengthInt = strlen(fileName);
    char* fileLengthString = numToStr(fileLengthInt);
    char success = (fileLengthInt < 1) ? 'f' : 's';
    char* buff = mallocStr(2 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + 1);
    sprintf(buff, "%c%c%s:%s%s:%s", success, 'f' , projectLengthString, projectName, fileLengthString, fileName);
    buff[2 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt] = '\0';
    printf("[fetchData] \"%s\"\n", buff);
    writeToSock(sockfd, buff);
}

void sendCommand(int sockfd, char* projectName, char* commandName){
    //Initialize projectLen to send
    int projectLengthInt = strlen(projectName);
    char* projectLengthString = numToStr(projectLengthInt);

    //Initialize commandLen to send
    int commandLengthInt = strlen(commandName);
    char* commandLengthString = numToStr(commandLengthInt);

    char success = (commandLengthInt < 1) ? 'f' : 's';
    char* buff = mallocStr(2 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(commandLengthString) + 1 + commandLengthInt + 1);
    sprintf(buff, "%c%c%s:%s%s:%s", success, 'c' , projectLengthString, projectName, commandLengthString, commandName);
    buff[2 +  strlen(projectLengthString) + 1 + projectLengthInt + strlen(commandLengthString) + 1 + commandLengthInt] = '\0';
    printf("[sendCommand] \"%s\"\n", buff);
    writeToSock(sockfd, buff);
}

char* getNextUnknownLen(int sockfd){
    char* message = mallocStr(65);
    bzero(message, 65*sizeof(char));
    int totalReadInBytes = 0;
    char* oldStr;
    do{
        char buff[2];
        bzero(buff, 2*sizeof(char));
        char* newRead = readFromSockIntoBuff(sockfd, buff, 1*sizeof(char));
        int newReadLen = strlen(newRead);
        totalReadInBytes+=newReadLen;
        if(newRead[0] == ':'){
            break;
        }
        else{
            memcpy(message+totalReadInBytes-1, newRead, 1*sizeof(char));
        }
    }while(message[totalReadInBytes]!= ':');
    //oldStr = message;
    //message = prependToStr(oldStr, firstRead);
    printf("[readInputProtocol] Length from server: \"%s\"\n", message);
    return message;
}




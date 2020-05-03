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

char** readInput(char* buff, int fd){
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
        return  (char**) getOutputArrFetched(success, commandType, projectName, fileName);
    }
    else if(commandType == 'c'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleSendCommand
        return (char**) getOutputArrFetched(success, commandType, projectName, fileName);
    }
    else{
        printf("Error: command type not recognized");
        return NULL;
    }
}





void sendData(int sockfd, char* projectName, char* fileName){
    int projectLengthInt = strlen(projectName);
    char* projectLengthString = numToStr(projectLengthInt);
    int fileLengthInt = strlen(fileName);
    char* fileLengthString = numToStr(fileLengthInt);
    char* data = getFileContents(fileName);
    //char* data = "file contents are right here";
    int dataLengthInt = strlen(data);
    char* dataLengthString = numToStr(dataLengthInt);
    char success = (dataLengthInt < 1) ? 'f' : 's';
    char* buff = mallocStr(2 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + strlen(dataLengthString) + 1 + dataLengthInt + 1);
    sprintf(buff, "%c%c%s:%s%s:%s%s:%s", success, 's', projectLengthString, projectName, fileLengthString, fileName, dataLengthString, data);
    buff[2 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + strlen(dataLengthString) + 1 + dataLengthInt] = '\0';
    printf("[send] \"%s\"\n", buff);
    
    int numBytesToWrite = strlen(buff);
    int numBytesWritten = 0;
    int totalNumBytesWritten = 0;
    while(numBytesToWrite > 0){
        numBytesWritten = write(sockfd, buff + totalNumBytesWritten, (numBytesToWrite)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
        totalNumBytesWritten+=numBytesWritten;
    }
    //write(sockfd, buff, sizeof(buff));
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
    printf("[fetch] \"%s\"\n", buff);

    int numBytesToWrite = strlen(buff);
    int numBytesWritten = 0;
    int totalNumBytesWritten = 0;
    while(numBytesToWrite > 0){
        numBytesWritten = write(sockfd, buff + totalNumBytesWritten, (numBytesToWrite)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
        totalNumBytesWritten+=numBytesWritten;
    }
    //write(sockfd, buff, sizeof(buff));
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
    printf("[send] \"%s\"\n", buff);

    int numBytesToWrite = strlen(buff);
    int numBytesWritten = 0;
    int totalNumBytesWritten = 0;
    while(numBytesToWrite > 0){
        numBytesWritten = write(sockfd, buff + totalNumBytesWritten, (numBytesToWrite)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
        totalNumBytesWritten+=numBytesWritten;
    }
    //write(sockfd, buff, sizeof(buff));
}

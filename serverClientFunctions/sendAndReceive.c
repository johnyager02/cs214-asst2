#include<stdlib.h>
#include<stdio.h>
#include"../recursiveD/stringFunc.h"
//#include"../recursiveD/stringFunc.c"
#include"../recursiveD/manifestFunc.h"
//#include"../recursiveD/manifestFunc.c"
#include<string.h>


void readInput(char*, int);

int main(int argc, char** argv){
    readInput("ss8:project18:thisfile20:thfufjdkslfjiweoiwfjjdskfl",20);
    send(1, "thisProject", "../recursiveD/proj0/test1");
    fetch(31, "newProject", ".Manifest");
    return 0;
}


void readInput(char* buff, int fd){
    char success = buff[0];
    if(success == 'f'){
        printf("[readInput] Error! command could not be executed\n");
        return;
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
    }
    else if(commandType == 'f'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleFetch
    }
    else if(commandType == 'c'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleSendCommand
    }
    else{
        printf("Error: command type not recognized");
        return;
    }
}


send(int sockfd, char* projectName, char* fileName){
    int projectLengthInt = strlen(projectName);
    char* projectLengthString = numToStr(projectLengthInt);
    int fileLengthInt = strlen(fileName);
    char* fileLengthString = numToStr(projectLengthInt);
    char* data = getFileContents(fileName);
    //char* data = "file contents are right here";
    int dataLengthInt = strlen(data);
    char* dataLengthString = numToStr(dataLengthInt);
    char success = (dataLengthInt < 1) ? 'f' : 's';
    char* buff = mallocStr(2 + 1 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + strlen(dataLengthString) + 1 + dataLengthInt + 1);
    sprintf(buff, "%c%c%s:%s%s:%s%s:%s", 's', success, projectLengthString, projectName, fileLengthString, fileName, dataLengthString, data);
    buff[2 + 1 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + strlen(dataLengthString) + 1 + dataLengthInt + 1] = '\0';
    printf("[send] %s\n", buff);
    //write(sockfd, buff, sizeof(buff))
}

fetch(int sockfd, char* projectName, char* fileName){
    int projectLengthInt = strlen(projectName);
    char* projectLengthString = numToStr(projectLengthInt);
    int fileLengthInt = strlen(fileName);
    char* fileLengthString = numToStr(projectLengthInt);
    char success = (fileLengthInt < 1) ? 'f' : 's';
    char* buff = mallocStr(2 + 1 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + 1);
    sprintf(buff, "%c%c%s:%s%s:%s", 's', success, projectLengthString, projectName, fileLengthString, fileName);
    buff[2 + 1 + strlen(projectLengthString) + 1 + projectLengthInt + strlen(fileLengthString) + 1 + fileLengthInt + 1] = '\0';
    printf("[fetch] %s\n", buff);
    //write(sockfd, buff, sizeof(buff))
}

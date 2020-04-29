#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include"stringFunc.h"

char* generateNewDirPath(char* oldPath, char* dirName){
    char* newDirPath1 = prependToStr(dirName, oldPath);
    char* newDirPath = appendToStr(newDirPath1, "/");
    free(newDirPath1);
    return newDirPath;
}

void recursivePrintDir(char* filepath){
    DIR* currentDirPtr = opendir(filepath);
    if(currentDirPtr == NULL){
        printf("DIR not found!\n");
        return;
    }
    struct dirent* currentPtr;
    readdir(currentDirPtr);
    readdir(currentDirPtr);
    currentPtr = readdir(currentDirPtr);
    while(currentPtr!=NULL){
        if(currentPtr->d_type == DT_REG){
            printf("Found File: %s\n", currentPtr->d_name);
        }
        else if(currentPtr->d_type == DT_DIR){
            char* dirName = currentPtr->d_name;
            printf("Found DIR: %s\n", dirName);
            char* newDirPath = generateNewDirPath(filepath, dirName);
            printf("Recursing into DIR: %s with new path: %s\n", dirName, newDirPath);
            recursivePrintDir(newDirPath);
            free(newDirPath);
        }
        currentPtr = readdir(currentDirPtr);
    }
    close(currentDirPtr);
}
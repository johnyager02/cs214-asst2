#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include"stringFunc.h"
#include"manifestFunc.h"

char* generateNewDirPath(char* oldPath, char* dirName){
    char* newDirPath1 = prependToStr(dirName, oldPath);
    char* newDirPath = appendToStr(newDirPath1, "/");
    free(newDirPath1);
    return newDirPath;
}

char* generateNewFilePath(char* oldPath, char* fileName){
    char* newDirPath = prependToStr(fileName, oldPath);
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

void printHashR(char* filepath){
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
            char* fileName = currentPtr->d_name;
            //printf("Found File: %s\n", currentPtr->d_name);
            char* newFilePath = generateNewFilePath(filepath, fileName);
            unsigned char* hash = getHash(newFilePath);
            printHash(hash, newFilePath);
            free(hash);
        }
        else if(currentPtr->d_type == DT_DIR){
            char* dirName = currentPtr->d_name;
            //printf("Found DIR: %s\n", dirName);
            char* newDirPath = generateNewDirPath(filepath, dirName);
            //printf("Recursing into DIR: %s with new path: %s\n", dirName, newDirPath);
            printHashR(newDirPath);
            free(newDirPath);
        }
        currentPtr = readdir(currentDirPtr);
    }
    close(currentDirPtr);
}

void recursiveDelete(char* filepath){
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
            char* fileName = currentPtr->d_name;
            //printf("Found File: %s\n", currentPtr->d_name);
            char* newFilePath = generateNewFilePath(filepath, fileName);
            printf("Deleting File: %s\n", newFilePath);
            remove(newFilePath);
            free(newFilePath);
        }
        else if(currentPtr->d_type == DT_DIR){
            char* dirName = currentPtr->d_name;
            //printf("Found DIR: %s\n", dirName);
            char* newDirPath = generateNewDirPath(filepath, dirName);
            //printf("Recursing into DIR: %s with new path: %s\n", dirName, newDirPath);
            recursiveDelete(newDirPath);
            printf("Deleting DIR: %s\n", newDirPath);
            rmdir(newDirPath);
            free(newDirPath);
        }
        currentPtr = readdir(currentDirPtr);
    }
    close(currentDirPtr);
}

int isEmptyDir(char* dirpath){
    int numFilesInDir = 0;
    DIR* currentDirPtr = opendir(dirpath);
    struct dirent* currentPtr;
    if (currentDirPtr == NULL){
        return 1;
    }
    while ((currentPtr = readdir(currentDirPtr)) != NULL) {
        if(++numFilesInDir > 2){break;}
    }
    closedir(currentDirPtr);
    if (numFilesInDir <= 2){//empty DIR
        return 1;
    } 
    return 0;
}
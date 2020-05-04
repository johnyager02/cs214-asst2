#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include"stringFunc.h"
#include"manifestFunc.h"
#include<sys/stat.h>

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

void recursiveMakeSubDir(char* filepath){
    if(existsDir(filepath) == 1){
        return;
    }
    //doesnt exist -> make dir for that
    int mkDir = mkdir(filepath, 0777);
    if(mkDir>= 0){
        printf("[recursiveMakeSubDir] Made DIR: \"%s\"\n", filepath);
        return;
    }
    //Can't make dir with given dirpath -> make subdir first
    char* nextDirPath = trimEndToDelim(filepath, '/');
    if(nextDirPath != NULL){
        //make subdirs first:
        recursiveMakeSubDir(nextDirPath);
        // make current dir
        mkDir = mkdir(filepath, 0777);
        printf("[recursiveMakeSubDir] Made DIR: \"%s\"\n", filepath);
    }
}

void createNewFile(char* filepath){
    if(existsFile(filepath) == 1){
        return;
    }
    //file doesn't exist create it

    int createFile;
    if((createFile = open(filepath, O_RDWR|O_CREAT, 00644)) < 0){
        //create failed -> make new subdirectories until can create;
        char* nextSubDirPath = trimEndToDelim(filepath, '/');
        recursiveMakeSubDir(nextSubDirPath);
        createFile = open(filepath, O_RDWR|O_CREAT, 00644);
        if(createFile > 0 ){
            printf("[writeNewFile] Made FILE: \"%s\"\n", filepath);
        } else{
            printf("[writeNewFile] Failed to make FILE: \"%s\"\n", filepath);
        }
    }
    return;
}


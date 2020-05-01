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

}

void checkout(char* projname){

}

void update(char* projname){

}

void upgrade(char* projname){

}

void commit(char* projname){

}

void push(char* projname){

}

void create(char* projname){

}

void destroy(char* projname){

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

void currentversion(char* projname){
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

void history(char* projname){

}

void rollback(char* projname, int version){
    
}
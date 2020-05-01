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

//NOTE! format of names in arguments for filenams/dirnames: "file1" || "sub0/subsub92/file716"

int existsFile(char* filename){ 
    int file = open(filename, O_RDONLY, 00644);
    if(file < 0){
        close(file);
        return 0;
    }
    else{
        close(file);
        return 1;
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
    if(existsFile(filename) != 1){ //Checks if file exists
        //Failed
        printf("[add] ERROR! This file: \"%s\" does not exist in the client's machine\n", filename);
        return;
    }
    char* manifestPath = appendToStr(projname, "/.Manifest");
    if(getFileLineManifest(manifestPath, filename, "-ps")!=NULL){//file already in manifest
        printf("[add] ERROR! This file: \"%s\" already exists in the Manifest\n", filename);
        return;
    } else{
        printf("[add] ERROR! This file: \"%s\" does not exist in the Manifest\n", filename);
    }

    //File exists and is not already in manifest -> append to .Manifest
    printf("[add] Adding file: \"%s\" to the manifest of project: \"%s\"\n", filename, projname);
    char* lineToAdd = getLineToAdd(0, filename);
    addToManifest(manifestPath, lineToAdd);
}

void removeEntry(char* projname, char* filename){

}

void currentversion(char* projname){
    //Connect with server -> get fileNames and fileversions for a target project as a char*:  "<success/fail><commandType><projectNameLength>:<projectName><fileNameLength>:<fileName>|<dataLength>:<data>|"

}

void history(char* projname){

}

void rollback(char* projname, int version){
    
}
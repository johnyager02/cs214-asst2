#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include<openssl/sha.h>
#include"stringFunc.h"
#include"recursiveD.h"

void printHash(unsigned char* hash, char* fileName){
    printf("Hash for file: \"%s\" is: ", fileName);
    int i;
    for(i = 0; i<20;i++){
        printf("%02x", hash[i]);
    }
    printf("\n");
}

unsigned char* mallocUChar(int size){
    unsigned char* str = (unsigned char*) malloc(size*sizeof(*str));
	if(str == NULL){//Failed to malloc
		printf("Error: %s\n", strerror(errno));
		while(str == NULL){
            str = (unsigned char*) malloc(size*sizeof(*str));
        }
	}
	return str;
}

unsigned char* getHash(char* filepath){
    unsigned char* hash = (unsigned char*) mallocUChar(20);
    memset(hash, '\0', 20*sizeof(*hash));
    unsigned char fileData[1024];
    memset(fileData, '\0', 1024*sizeof(unsigned char));
    int file = open(filepath, O_RDONLY);
    if(file < 0){//error opening file specified
		printf("Fatal Error: %s in regard to path %s\n", strerror(errno), filepath);
        exit(EXIT_FAILURE);
		return NULL;
	}
    printf("Successfully opened file: %s\n", filepath);
	int numReads = 0;
	int numBytesRead= 0;
    int totalReadInBytesPtr = 0;
	
    SHA_CTX context;
    SHA1_Init(&context);

    do{
        //printf("Entering loop\n");
        numBytesRead = read(file, fileData, 1024*sizeof(*hash));
        if(numBytesRead< 0){
            printf("Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(file);
            free(hash);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        else if(numBytesRead == 0 && numReads == 0){ //empty file, return empty hash "da39a3ee5e6b4b0d3255bfef95601890afd80709"
            unsigned char emptyHash[] = "";
            SHA1_Update(&context, emptyHash, 0);
            SHA1_Final(hash, &context);
            return hash;
        }
        else if(numBytesRead > 0){
            //printf("Buffer so far is: %s", fileData);
            SHA1_Update(&context, fileData, numBytesRead);
            totalReadInBytesPtr+=numBytesRead;
         numReads++;
        }
    } while(numBytesRead > 0);
	
    printf("Done reading file: %s\n", filepath);
    //Done reading file... ->finalize sha1
    SHA1_Final(hash, &context);

	close(file);
	return hash;
}

char* getLineToAdd(int versionNum, char* filepath){ //filepath should be "./proj0/file0"
    int versionLen = snprintf(NULL, 0, "%d", versionNum);
    char* versionStr = (char*) mallocStr(versionLen + 1);
    memset(versionStr + versionLen, '\0', 1*sizeof(*versionStr));
    snprintf(versionStr, versionLen + 1, "%d", versionNum);

    unsigned char* hash = getHash(filepath);
    printHash(hash, filepath);
    char* hashStr = (char*) mallocStr(40 + 1);
    memset(hashStr, '\0', 41*sizeof(char));
    int i;
    for (i=0; i < 20 ; i++) {
        sprintf( &(hashStr[i+i]), "%02x", hash[i]);
    }
    //printf("%s\n", hashStr);

    int finalLineLen = 46 + versionLen + strlen(filepath); // 3 for spaces, 1 for serverCheckChar, 1 for '\n', 1 for null terminator, and 40 for hash in "%02x" form
    char* finalLine = (char*) mallocStr(finalLineLen); 
    memset(finalLine, '\0', finalLineLen * sizeof(*finalLine));
    memcpy(finalLine, versionStr, versionLen*sizeof(char));
    char* oldStr = finalLine;
    finalLine = appendToStr(oldStr, " ");
    free(oldStr); oldStr = finalLine;
    finalLine = appendToStr(oldStr, filepath);
    free(oldStr); oldStr = finalLine;
    finalLine = appendToStr(oldStr, " ");
    free(oldStr); oldStr = finalLine;
    finalLine = appendToStr(oldStr, hashStr);
    free(oldStr); oldStr = finalLine;
    finalLine = appendToStr(oldStr, " ");
    free(oldStr); oldStr = finalLine;
    finalLine = appendToStr(oldStr, "n");
    free(oldStr); oldStr = finalLine;
    finalLine = appendToStr(oldStr, "\n");
    memset(finalLine + finalLineLen - 1, '\0', 1*sizeof(char));
    free(oldStr);
    free(versionStr);
    free(hashStr);
    printf("finalLine is: %s", finalLine);
    return finalLine;
}

void initializeManifest(char* projectpath){ // project path should be "./proj0"
    char manifestStr[] = ".Manifest";
    char* manifestPath = appendToStr(projectpath, "/");
    char* oldStr = manifestPath;
    manifestPath = appendToStr(oldStr, manifestStr);
    free(oldStr);
    int newManifest = open(manifestPath, O_RDWR | O_CREAT, 00644);

    int versionLen = snprintf(NULL, 0, "%d", 0);
    char* versionStr = (char*) mallocStr(versionLen + 1);
    memset(versionStr + versionLen, '\0', 1*sizeof(*versionStr));
    snprintf(versionStr, versionLen + 1, "%d", 0);
    char* versionWithNewline = appendToStr(versionStr, "\n");
    free(versionStr);
    int numBytesWritten = write(newManifest, versionWithNewline, strlen(versionWithNewline)*sizeof(char));
    
}

void addToManifest(char* manifestPath, char* line){ //path should be "./<currentProjName>/.Manifest"
    int file = open(manifestPath, O_WRONLY | O_APPEND, 00644);
    if(file<0){
        printf("Fatal Error: %s in regard to path %s\n", strerror(errno), manifestPath);
        exit(EXIT_FAILURE);
    }
    int numBytesToWrite = strlen(line);
    int numBytesWritten = 0;
    while(numBytesToWrite > 0){
        numBytesWritten = write(file, line, strlen(line)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
    }
}

void modifyManifest(char* manifestPath){

}



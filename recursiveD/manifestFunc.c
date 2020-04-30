#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include<openssl/sha.h>
#include"stringFunc.h"
#include"recursiveD.h"

char* hashToStr(unsigned char* hash){
    char* hashStr = (char*) mallocStr(40 + 1);
    memset(hashStr, '\0', 41*sizeof(char));
    int i;
    for (i=0; i < 20 ; i++) {
        sprintf( &(hashStr[i+i]), "%02x", hash[i]);
    }
    return hashStr;
}

char* numToStr(int num){
    int versionLen = snprintf(NULL, 0, "%d", num);
    char* versionStr = (char*) mallocStr(versionLen + 1);
    memset(versionStr + versionLen, '\0', 1*sizeof(*versionStr));
    snprintf(versionStr, versionLen + 1, "%d", num);
    return versionStr;
}

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
    char* versionStr = numToStr(versionNum);
    int versionLen = strlen(versionStr);

    unsigned char* hash = getHash(filepath);
    char* hashStr = (char*) hashToStr(hash);
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


char* getHashStrLine(char* line){ // assumes correctly formatted line: "<version> <./proj0/test0> <hash> <checkedByServer>\n\0"
    printf("[getHashStrLine] input line is: \"%s\"\n", line);
    char* hashStr = (char*) mallocStr(40 + 1);
    memset(hashStr, '\0', 41*sizeof(char));
    int numZeros = 0;
    int i = 0;
    char currentChar = line[0];
    int hashIndex = 0;
    printf("[getHashStrLine] read: \"");
    while(currentChar != '\n'){
        currentChar = line[i];
        printf("%c", currentChar);
        if(currentChar == ' '){
            numZeros++;
        }
        else if(numZeros == 2 && currentChar != ' '){
            memcpy(hashStr + hashIndex, line+ i, 1*sizeof(char));
            hashIndex++;
        }
        i++;
    }
    printf("\" [getHashStrLine] Loop stop\n");
    printf("[getHashStrLine] numZeros is: %d\n", numZeros);
    if(numZeros > 2){
        //printf("[getHashStrLine] hashStr is: \"%s\"\n", hashStr);
        return hashStr;
    }
    return NULL;
}

char* getFilePathStrLine(char* line){ // assumes correctly formatted line: "<version> <./proj0/test0> <hash> <checkedByServer>\n\0"
    char* finalStr = (char*) mallocStr(strlen(line));
    memset(finalStr, '\0', strlen(line)*sizeof(char));
    int i;
    int numSpaces = 0;
    int finalStrIndex = 0;
    for(i = 0; i<strlen(line);i++){
        if(line[i] == ' '){
            numSpaces++;
            if(numSpaces == 2){
                return finalStr;
            }
        }
        else if(numSpaces == 1){
            //start reading filepath into str
            memcpy(finalStr + finalStrIndex, line + i, 1*sizeof(char));
            finalStrIndex++;
        }
    }
    return NULL;
}



char* getFileLineManifest(char* manifestPath, char* filepath, char* searchFlag){ // filepath: "./proj0/test0"... returns line for a specific file, returns null if file not found
    int manifestFile = open(manifestPath, O_RDONLY, 00644);
    if(manifestFile<0){
        printf("Fatal Error: %s in regard to path %s\n", strerror(errno), manifestPath);
        exit(EXIT_FAILURE);
    }
    //File exists -> Do the parsing by line
    char* targetHashStr;
    char* currentHashStr;
    int numBytesRead = 0;
    int totalReadInBytes = 0;
    int linesRead = 0;
    char* finalLine = (char*) mallocStr(1025);
    memset(finalLine, '\0', 1025*sizeof(char));
    int currentBufferSize = 1024;
    do{
        numBytesRead = read(manifestFile, finalLine+totalReadInBytes, 1*sizeof(char));
        totalReadInBytes+=numBytesRead;
        if(totalReadInBytes == currentBufferSize){
            finalLine = (char*) reallocStr(finalLine, 2*currentBufferSize + 1);
            currentBufferSize = 2*currentBufferSize;
            memset(finalLine, '\0', (currentBufferSize + 1)*sizeof(char));
        }
        if(numBytesRead < 0){
            printf("Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(manifestFile);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        else if(numBytesRead == 0 && linesRead == 0){ // empty file
            printf("Empty file!\n");
            return NULL;
        }
        else if(numBytesRead == 1){
            char lastCharRead = finalLine[totalReadInBytes-1];
            if(lastCharRead == '\n'){ // got newLine, compare hash
                char* currentLine = finalLine;
                printf("[getFileLineManifest] current Buffer is: %s\n", currentLine);

                if(compareString(searchFlag, "-ps") == 0){
                    printf("[getFileLineManifest] targetStr is: \"%s\"\n", filepath);
                    char* currentFileStr = getFilePathStrLine(currentLine);
                    if(currentFileStr!=NULL){
                        printf("[getFileLineManifest] currentFileStr is: \"%s\"\n", currentFileStr);
                        if(compareString(filepath, currentFileStr) == 0){
                            printf("[getFileLineManifest] Found filepath! Returning line: \"%s\"\n", currentLine);
                            return currentLine;
                        }
                    }
                }
                else if(compareString(searchFlag, "-pi") == 0){
                    printf("[getFileLineManifest] targetStr is: \"%s\"\n", filepath);
                    char* currentFileStr = getFilePathStrLine(currentLine);
                    if(currentFileStr!=NULL){
                        printf("[getFileLineManifest] currentFileStr is: \"%s\"\n", currentFileStr);
                        if(compareString(filepath, currentFileStr) == 0){
                            printf("[getFileLineManifest] Found filepath! Returning lineNum: \"%d\"\n", linesRead+1);
                            return numToStr(linesRead+1);
                        }
                    }
                }
                else if(compareString(searchFlag, "-hs") == 0){
                    char* filehash = getHash(filepath);
                    targetHashStr = hashToStr(filehash);
                    printf("[getFileLineManifest] targetHash is: \"%s\"\n", targetHashStr);
                    currentHashStr = getHashStrLine(currentLine);
                    if(currentHashStr!= NULL){ //File hash found in currentLine -> compareHashes
                        printf("[getFileLineManifest] currentHashStr is: \"%s\"\n", currentHashStr);
                        if(compareString(targetHashStr, currentHashStr) == 0){
                            printf("[getFileLineManifest] Found hash! Returning line: \"%s\"\n", currentLine);
                            return currentLine;
                        }
                    }
                }
                else if(compareString(searchFlag, "-hi") == 0){
                    char* filehash = getHash(filepath);
                    targetHashStr = hashToStr(filehash);
                    printf("[getFileLineManifest] targetHash is: \"%s\"\n", targetHashStr);
                    currentHashStr = getHashStrLine(currentLine);
                    if(currentHashStr!= NULL){ //File hash found in currentLine -> compareHashes
                        printf("[getFileLineManifest] currentHashStr is: \"%s\"\n", currentHashStr);
                        if(compareString(targetHashStr, currentHashStr) == 0){
                            printf("[getFileLineManifest] Found hash! Returning lineNum: \"%d\"\n", linesRead+1);
                            return numToStr(linesRead+1);
                        }
                    }
                }


                //reset buffer
                memset(finalLine, '\0', currentBufferSize+1);

                totalReadInBytes = 0; // start reading from beginning of buffer again
                linesRead++;
            }
        }
    } while(numBytesRead != 0);
    close(manifestFile);
    return NULL;
}

void writeAfterChar(char* filepath, int lineNum, char* change, char charToSkip, int numSkipsToDo){ //Ex) Used to write after the first space; lineNum: first line is lineNum 0 
    int file = open(filepath, O_RDWR, 00644);
    if(file<0){
        printf("Fatal Error: %s in regard to path %s\n", strerror(errno), file);
        exit(EXIT_FAILURE);
    }
    //File exists -> go to lineNum
    int numBytesRead = 0;
    int linesRead = 0;
    char buffer[2];
    memset(buffer, '\0', 2*sizeof(char));
    do{
        numBytesRead = read(file, buffer, 1*sizeof(char));
        if(numBytesRead < 0){
            printf("Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(file);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        else if(numBytesRead == 0 && linesRead == 0){ // empty file
            printf("Empty file!\n");
            return;
        }
        else if(numBytesRead == 1){
            if( buffer[0] == '\n'){ // got newLine
                linesRead++;
            }
        }
    } while(numBytesRead != 0 && linesRead < lineNum);

    //Now at lineNum specified -> start skipping chars specified:
    int numBytesToWrite = strlen(change);
    int numBytesWritten = 0;
    numBytesRead = 0;
    int numChars = 0;

    if(numSkipsToDo == 0){
        while(numBytesToWrite > 0){
            numBytesWritten = write(file, change, strlen(change)*sizeof(char));
            numBytesToWrite-=numBytesWritten;
        }
        close(file);
        return;
    }

    do{
        numBytesRead = read(file, buffer, 1*sizeof(char));
        if(numBytesRead < 0){
            printf("Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(file);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        exit(EXIT_FAILURE);
        }
        else if(numBytesRead == 1){
            if(buffer[0] == charToSkip){ 
                numChars++;
            }
        }
    } while(numBytesRead != 0 && numChars < numSkipsToDo);
    
    //Do specified change:
    while(numBytesToWrite > 0){
        numBytesWritten = write(file, change, strlen(change)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
    }
    close(file);
}

void modifyManifest(char* manifestPath, int lineNum, char* flagChange, char* change){ // lineNum: first line is lineNum 0
    //Do the changes at the specified lineNum:
    int numSpacesLimit;
    if(compareString(flagChange, "-v") == 0){ // update Version
        //Skip 1 space
        numSpacesLimit = 0;
    }
    else if(compareString(flagChange, "-p") == 0){ // update Path
        //Skip 1 space
        numSpacesLimit = 1;
    }
    else if(compareString(flagChange, "-h") == 0){ // update Hash
        //Skip 2 spaces
        numSpacesLimit = 2;
    }
    else if(compareString(flagChange, "-c") == 0){ // update checkedByServer
        //Skip 3 spaces
        numSpacesLimit = 3;
    }
    writeAfterChar(manifestPath, lineNum, change, ' ', numSpacesLimit);
    return;
}
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include<openssl/sha.h>
#include"stringFunc.h"
#include"recursiveD.h"
#include"WTF.h"

void overwriteOrCreateFile(char* filepath, char* newdata){
    int file = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 00644);
    int numBytesToWrite = strlen(newdata);
    int numBytesWritten = 0;
    int totalNumBytesWritten = 0;
    while(numBytesToWrite > 0){
        numBytesWritten = write(file, newdata + totalNumBytesWritten, (numBytesToWrite-totalNumBytesWritten)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
        totalNumBytesWritten+=numBytesWritten;
    }
    close(file);
}

char* nthToken(char* str, int n, char delimiter){ //given str -> return nth token EX) "./proj0/dir0/" w/ n=2-> "dir0"; n is like array index, n=0 is first dirname "."
    char* token = (char*) mallocStr(strlen(str));
    memset(token, '\0', strlen(str)*sizeof(char));

    //
    int i;
    int indexOfCopy = 0;
    int numChar = 0;
    for(i = 0; i<strlen(str);i++){
        if(str[i] == delimiter){//reset buffer
            if(numChar== n){
                return token;
            }
            memset(token, '\0', strlen(str)*sizeof(char));
            indexOfCopy = 0;
            numChar++;
        }
        else{
            memcpy(token + indexOfCopy, str + i, 1*sizeof(char));
            indexOfCopy++;
            if(i+1 == strlen(str) && n==numChar){
                return token;
            }
        }
    }
    return NULL;
}

char* getProjVersion(char* projname){//Finds the manifest for project and returns the project's version as a string
    char manifestStr[] = ".Manifest";
    char* manifestPath = appendToStr(projname, "/");
    char* oldStr = manifestPath;
    manifestPath = appendToStr(oldStr, manifestStr);
    free(oldStr);
    int manifestFile = open(manifestPath, O_RDONLY, 00644);
    if(manifestFile<0 && errno == ENOENT){
        printf("[getProjVersion] Manifest does not exist in regard to project: \"%s\"\n", projname);
        return NULL;
    }

    //successfully opened manifest for target project -> get the version
    
    //initialize buffer
    char* projVersion = (char*) mallocStr(1025);
    memset(projVersion, '\0', 1025*sizeof(char));

    int numBytesRead = 0;
    int totalReadInBytes = 0;
    //int indexToCopy = 0;
    int currentBufferSize = strlen(projVersion);
    do{
        numBytesRead = read(manifestFile, projVersion + totalReadInBytes, 1*sizeof(char));
        totalReadInBytes+=numBytesRead;
        if(totalReadInBytes == currentBufferSize){// Realloc buffer
            projVersion = (char*) reallocStr(projVersion, 2*currentBufferSize + 1);
            currentBufferSize = 2*currentBufferSize;
            memset(projVersion + totalReadInBytes, '\0', (currentBufferSize + 1 - totalReadInBytes)*sizeof(char));
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
        else if(numBytesRead == 1){
            if(projVersion[totalReadInBytes - 1] == '\n'){
                memset(projVersion + (totalReadInBytes - 1), '\0', 1*sizeof(char));
                close(manifestFile);
                return projVersion;
            }
        }
        //indexToCopy++;
    }while(numBytesRead!=0);
    close(manifestFile);
    return NULL;
}


char* getFileContents(char* filepath){
    int file = open(filepath, O_RDONLY, 00644);
    if(file<0){
        printf("Fatal Error: %s in regard to path %s\n", strerror(errno), file);
        exit(EXIT_FAILURE);
    }
    //Initialize buffer
    char* finalStr = (char*) mallocStr(1025);
    memset(finalStr, '\0', 1025*sizeof(char));

    //Init variables
    int numBytesRead = 0;
    int totalReadInBytes  = 0;
    int currentBufferSize = 1024;

    //Start reading into buffer
    do{
        //printf("Current buffer is: \"%s\"\n", finalStr);
        numBytesRead = read(file, finalStr+totalReadInBytes, 1*sizeof(char));
        totalReadInBytes+=numBytesRead;
        if(totalReadInBytes == currentBufferSize){// Realloc buffer
            finalStr = (char*) reallocStr(finalStr, 2*currentBufferSize + 1);
            currentBufferSize = 2*currentBufferSize;
            memset(finalStr + totalReadInBytes, '\0', (currentBufferSize + 1 - totalReadInBytes)*sizeof(char));
        }
        if(numBytesRead < 0){//Error reading file
            printf("Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(file);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
    }while(numBytesRead != 0);
    close(file);
    //Make buffer exactly len of contents + 1 number of bytes long to be consistent
    int oldNumBytes = strlen(finalStr);
    finalStr = (char*) reallocStr(finalStr, (oldNumBytes + 1)*sizeof(char));

    printf("[getFileContents]-Final buffer is: \"%s\"\n", finalStr);
    return finalStr;
}

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
		printf("[getHash]-Fatal Error: %s in regard to path %s\n", strerror(errno), filepath);
        exit(EXIT_FAILURE);
		return NULL;
	}
    //printf("Successfully opened file: %s\n", filepath);
	int numReads = 0;
	int numBytesRead= 0;
    int totalReadInBytes = 0;
	
    SHA_CTX context;
    SHA1_Init(&context);

    do{
        //printf("Entering loop\n");
        numBytesRead = read(file, fileData, 1024*sizeof(*hash));
        totalReadInBytes+=numBytesRead;
        if(numBytesRead< 0){
            printf("[getHash]-Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(file);
            free(hash);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        else if(numBytesRead == 0 && totalReadInBytes == 0){ //empty file, return empty hash "da39a3ee5e6b4b0d3255bfef95601890afd80709"
            unsigned char emptyHash[] = "";
            SHA1_Update(&context, emptyHash, 0);
            SHA1_Final(hash, &context);
            return hash;
        }
        else if(numBytesRead > 0){
            //printf("Buffer so far is: %s", fileData);
            SHA1_Update(&context, fileData, numBytesRead);
            totalReadInBytes+=numBytesRead;
         numReads++;
        }
    } while(numBytesRead > 0);
	
    //printf("Done reading file: %s\n", filepath);
    //Done reading file... ->finalize sha1
    SHA1_Final(hash, &context);

	close(file);
	return hash;
}

char* getLineToAdd(int versionNum, char* filepath){ //filepath should be "./proj0/file0" Or "proj0/file0" OR ""
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

void addToManifest(char* manifestPath, char* line){ //path should be "./<currentProjName>/.Manifest" !!!!!MAKE SURE TO APPEND \n TO THE INPUTLINE"
    int file = open(manifestPath, O_WRONLY | O_APPEND, 00644);
    if(file<0){
        printf("Fatal Error: %s in regard to path %s\n", strerror(errno), manifestPath);
        exit(EXIT_FAILURE);
    }
    int numBytesToWrite = strlen(line);
    int numBytesWritten = 0;
    int totalNumBytesWritten = 0;
    while(numBytesToWrite > 0){
        numBytesWritten = write(file, line + totalNumBytesWritten, (numBytesToWrite-totalNumBytesWritten)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
        totalNumBytesWritten+=numBytesWritten;
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
    char* linesReadStr;
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
            memset(finalLine + totalReadInBytes, '\0', (currentBufferSize + 1 -totalReadInBytes)*sizeof(char));
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
        else if(numBytesRead == 0 && totalReadInBytes == 0){ // empty file
            //printf("Empty file!\n");
            return NULL;
        }
        else if(numBytesRead == 1){
            char lastCharRead = finalLine[totalReadInBytes-1];
            if(lastCharRead == '\n'){ // got newLine, compare hash
                char* currentLine = finalLine;
                //printf("[getFileLineManifest] current Buffer is: %s\n", currentLine);

                if(compareString(searchFlag, "-ps") == 0){
                   // printf("[getFileLineManifest] targetStr is: \"%s\"\n", filepath);
                    char* currentFileStr = getFilePathStrLine(currentLine);
                    if(currentFileStr!=NULL){
                        //printf("[getFileLineManifest] currentFileStr is: \"%s\"\n", currentFileStr);
                        if(compareString(filepath, currentFileStr) == 0){
                            printf("[getFileLineManifest] Found filepath! Returning line: \"%s\"\n", currentLine);
                            return currentLine;
                        }
                    }
                }
                else if(compareString(searchFlag, "-pi") == 0){
                    //printf("[getFileLineManifest] targetStr is: \"%s\"\n", filepath);
                    char* currentFileStr = getFilePathStrLine(currentLine);
                    if(currentFileStr!=NULL){
                        //printf("[getFileLineManifest] currentFileStr is: \"%s\"\n", currentFileStr);
                        if(compareString(filepath, currentFileStr) == 0){
                            printf("[getFileLineManifest] Found filepath! Returning lineNum: \"%d\"\n", linesRead+1);
                            return numToStr(linesRead);
                        }
                    }
                }
                else if(compareString(searchFlag, "-hs") == 0){
                    char* filehash = getHash(filepath);
                    targetHashStr = hashToStr(filehash);
                    //printf("[getFileLineManifest] targetHash is: \"%s\"\n", targetHashStr);
                    currentHashStr = getHashStrLine(currentLine);
                    if(currentHashStr!= NULL){ //File hash found in currentLine -> compareHashes
                        //printf("[getFileLineManifest] currentHashStr is: \"%s\"\n", currentHashStr);
                        if(compareString(targetHashStr, currentHashStr) == 0){
                            printf("[getFileLineManifest] Found hash! Returning line: \"%s\"\n", currentLine);
                            return currentLine;
                        }
                    }
                }
                else if(compareString(searchFlag, "-hi") == 0){
                    char* filehash = getHash(filepath);
                    targetHashStr = hashToStr(filehash);
                    //printf("[getFileLineManifest] targetHash is: \"%s\"\n", targetHashStr);
                    currentHashStr = getHashStrLine(currentLine);
                    if(currentHashStr!= NULL){ //File hash found in currentLine -> compareHashes
                        //printf("[getFileLineManifest] currentHashStr is: \"%s\"\n", currentHashStr);
                        if(compareString(targetHashStr, currentHashStr) == 0){
                            printf("[getFileLineManifest] Found hash! Returning lineNum: \"%d\"\n", linesRead+1);
                            return numToStr(linesRead);
                        }
                    }
                }
                else if(searchFlag[1] == 'l'){ // return line by linenumber
                    char* lineTarget = (char*) mallocStr(strlen(searchFlag) - 1);
                    memset(lineTarget, '\0', (strlen(searchFlag) - 1)*sizeof(char));
                    memcpy(lineTarget, searchFlag + 2, (strlen(searchFlag) - 2) * sizeof(char));
                    linesReadStr =numToStr(linesRead);
                    if(compareString(lineTarget, linesReadStr) == 0){
                        printf("[getFileLineManifest] Found target line number! Returning line: \"%s\"\n", currentLine);
                        free(lineTarget);
                        free(linesReadStr);
                        return currentLine;
                    }
                }

                //reset buffer
                memset(finalLine, '\0', currentBufferSize+1);

                totalReadInBytes = 0; // start reading from beginning of buffer again
                linesRead++;
            }
        }
    } while(numBytesRead != 0);
    free(linesReadStr);
    close(manifestFile);
    return NULL;
}

char* getLineFile(char* filepath, int lineNum){
    char* lineNumStr = numToStr(lineNum);
    char* lineTarget = prependToStr(lineNumStr, "-l");
    free(lineNumStr);
    return getFileLineManifest(filepath, filepath, lineTarget);
    
}

void setLineFile(char* filepath, int lineNum, char* newline){
    int file = open(filepath, O_RDONLY, 00644);
    if(file<0){
        printf("[setLineFile] Fatal Error: %s in regard to path %s\n", strerror(errno), file);
        return;
    }

    //goToLineNumber, then write change
    int numBytesRead = 0;
    int linesRead = 0;
    int totalReadInBytes = 0;
    char* strToWrite = (char*) mallocStr(1025);
    memset(strToWrite, '\0', 1025*sizeof(char));
    char* strToWriteFinal;
    //Init variables
    int currentBufferSize = 1024;
    int indexOfCopy = 0;
    int skippedUnwantedLine = 0;
    //Copy everything before line to change into buffer
    do{
        // if(lineNum == 0){
        //     break;
        // }
        //printf("[setLineFile] Current Buffer size is: %d\n", currentBufferSize);
        numBytesRead = read(file, strToWrite+ indexOfCopy, 1*sizeof(char));
        totalReadInBytes+=numBytesRead;
        if(totalReadInBytes == currentBufferSize){// Realloc buffer
            strToWrite = (char*) reallocStr(strToWrite, 2*currentBufferSize + 1);
            currentBufferSize = 2*currentBufferSize;
            memset(strToWrite + totalReadInBytes, '\0', (currentBufferSize + 1 - totalReadInBytes)*sizeof(char));
        }
        if(numBytesRead < 0){
            printf("Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(file);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        else if(numBytesRead == 0 && totalReadInBytes == 0){ // empty file
            printf("[writeChangeToLine] Empty file!\n");
            return;
        }
        else if(numBytesRead == 1){
            if( strToWrite[indexOfCopy] == '\n'){ // got newLine
                linesRead++;
                if(linesRead == lineNum){
                    strToWriteFinal = (char*) mallocStr(strlen(strToWrite)+1);
                    memset(strToWriteFinal, '\0', (strlen(strToWrite) +1)*sizeof(char));
                    memcpy(strToWriteFinal, strToWrite, (strlen(strToWrite))*sizeof(char));
                    memset(strToWrite, '\0', (currentBufferSize +1)*sizeof(char));
                    indexOfCopy = 0;
                    continue;
                }
                else if(linesRead> lineNum && skippedUnwantedLine == 0){
                    skippedUnwantedLine = 1;
                    memset(strToWrite, '\0', (currentBufferSize +1)*sizeof(char));
                    indexOfCopy = 0;
                    continue;
                }
            }
            indexOfCopy++;
        }
    } while(numBytesRead != 0);
    printf("[setLineFile] Total bytes read is: %d\n", totalReadInBytes);
    printf("[setLineFile] poststrToWrite  is: \"%s\"\n", strToWrite);
    if(lineNum == 0){
        //char* old = strToWrite;
        strToWriteFinal = prependToStr(strToWrite, newline);
        //free(old);
    } 
    else{
        //linesRead == lineNum -> append to buffer newline;
        char* oldStr1 = strToWriteFinal;
        strToWriteFinal = appendToStr(strToWriteFinal, newline);
        free(oldStr1);
        char* oldStr = strToWriteFinal;
        strToWriteFinal = appendToStr(oldStr, strToWrite);
        free(oldStr);
        printf("[setLineFile] strToWriteFinal after appending  is: \"%s\"\n", strToWriteFinal);    
    }
    
    close(file);
    printf("[setLineFile] strToWriteFinal is: \"%s\"\n", strToWriteFinal);

    //Now write strToWrite to file using O_TRUNC
    int fileToWrite = open(filepath, O_RDWR | O_TRUNC, 00644);
    if(fileToWrite<0){
        printf("[setLineFile] Fatal Error: %s in regard to path %s\n", strerror(errno), fileToWrite);
        return;
    }
    int numBytesToWrite = strlen(strToWriteFinal);
    int numBytesWritten = 0;
    while(numBytesToWrite > 0){
        numBytesWritten = write(fileToWrite, strToWriteFinal, strlen(strToWriteFinal)*sizeof(char));
        numBytesToWrite-=numBytesWritten;
    }
    free(strToWrite);
    free(strToWriteFinal);
    close(fileToWrite);
}


void removeLine(char* filepath, int lineNum){
    //Prep file reading
    int fileRead = open(filepath, O_RDONLY, 00644);
    if(fileRead<0){
        printf("Fatal Error: %s in regard to path %s\n", strerror(errno), fileRead);
        exit(EXIT_FAILURE);
    }
    //Prep file writing (separate file descriptor)
    int fileWrite = open(filepath, O_WRONLY, 00644);
    if(fileWrite<0){
        printf("Fatal Error: %s in regard to path %s\n", strerror(errno), fileWrite);
        exit(EXIT_FAILURE);
    }

    //File exists -> do line by line write, except line to remove
    int numBytesRead = 0;
    int linesRead = 0;
    char buffer[2];
    memset(buffer, '\0', 2*sizeof(char));
    int numBytesWritten = 0;
    int lenNewFile = 0;
    int totalReadInBytes = 0;
    do{
        numBytesRead = read(fileRead, buffer, 1*sizeof(char));
        totalReadInBytes+=numBytesRead;
        if(numBytesRead < 0){
            printf("Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(fileRead);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            fileclose = close(fileWrite);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        else if(numBytesRead == 0 && totalReadInBytes == 0){ // empty file
            printf("Empty file!\n");
            return;
        }
        else if(numBytesRead == 1){
            if( buffer[0] == '\n'){ // got newLine
                if(linesRead!=lineNum){
                     //write to file the byte in buffer
                    numBytesWritten+= write(fileWrite, buffer, 1*sizeof(char)); 
                    lenNewFile++;
                }
                linesRead++;
            }
            else if(linesRead!=lineNum){ //Not the line we want to remove
                //write to file the byte in buffer
                numBytesWritten+= write(fileWrite, buffer, 1*sizeof(char));
                lenNewFile++;
            }
        }
    } while(numBytesRead != 0);
    truncate(filepath, lenNewFile);
    close(fileRead);
    close(fileWrite);
}

void modifyManifest(char* projname, int lineNum, char* flagChange, char* change){ // lineNum: first line is lineNum 0
    char manifestStr[] = ".Manifest";
    char* manifestPath = appendToStr(projname, "/");
    char* oldStr = manifestPath;
    manifestPath = appendToStr(oldStr, manifestStr);
    free(oldStr);
    //Get line at linenum:
    char* lineToChange = getLineFile(manifestPath, lineNum);
    char* oldVersion = nthToken(lineToChange, 0, ' ');
    char* oldPath = nthToken(lineToChange, 1, ' ');
    char* oldHash = nthToken(lineToChange, 2, ' ');
    char* oldCheckChar = nthToken(lineToChange, 3, ' ');
    
    //Do the changes at the specified lineNum: EX) "0 ./proj0/test0 asdasasdsdsda n\n"
    char* newline = NULL;
    if(compareString(flagChange, "-v") == 0){ // update Version
        printf("[modifyManifest]-update version to : %s\n", change);
        int newLineSize = strlen(lineToChange) + strlen(change);
        newline = (char*) mallocStr(newLineSize + 1);
        memset(newline, '\0', (newLineSize+1)*sizeof(char));
        sprintf(newline, "%s %s %s %s", change, oldPath, oldHash, oldCheckChar);
    }
    else if(compareString(flagChange, "-p") == 0){ // update Path
        printf("[modifyManifest]-update path to : %s\n", change);
        int newLineSize = strlen(lineToChange) + strlen(change);
        newline = (char*) mallocStr(newLineSize + 1);
        memset(newline, '\0', (newLineSize+1)*sizeof(char));
        sprintf(newline, "%s %s %s %s", oldVersion, change, oldHash, oldCheckChar);
    }
    else if(compareString(flagChange, "-h") == 0){ // update Hash
        printf("[modifyManifest]-update hash to : %s\n", change);
        int newLineSize = strlen(lineToChange) + strlen(change);
        newline = (char*) mallocStr(newLineSize + 1);
        memset(newline, '\0', (newLineSize+1)*sizeof(char));
        sprintf(newline, "%s %s %s %s", oldVersion, oldPath, change, oldCheckChar);
    }
    else if(compareString(flagChange, "-c") == 0){ // update checkedByServer
        change = (char*) appendToStr(change, "\n");
        printf("[modifyManifest]-update checkByServer to : %s\n", change);
        int newLineSize = strlen(lineToChange) + strlen(change);
        newline = (char*) mallocStr(newLineSize + 1);
        memset(newline, '\0', (newLineSize+1)*sizeof(char));
        sprintf(newline, "%s %s %s %s", oldVersion, oldPath, oldHash, change);
        free(change);
    }

    if(newline != NULL){
    //Parse old line:
    setLineFile(manifestPath, lineNum, newline);
    }
    free(lineToChange);
    free(oldVersion);
    free(oldPath);
    free(oldHash);
    free(oldCheckChar);
    free(newline);
    free(manifestPath);
    return;
}

void setProjVersion(char* projname, char* version){
    char manifestStr[] = ".Manifest";
    char* manifestPath = appendToStr(projname, "/");
    char* oldStr = manifestPath;
    manifestPath = appendToStr(oldStr, manifestStr);
    free(oldStr);
    int manifestFile = open(manifestPath, O_RDONLY, 00644);
    if(manifestFile<0 && errno == ENOENT){
        printf("[setProjVersion] Manifest does not exist in regard to project: \"%s\"\n", projname);
        return;
    }
    close(manifestFile);

    //successfully opened manifest -> change project version
    modifyManifest(manifestPath, 0, "-v", version);
}

int getNumLines(char* filepath){
    int linesRead = 0;
    int file = open(filepath, O_RDONLY);
    if(file<0){
        printf("[getNumLines] Error: %s in regard to path %s\n", strerror(errno), file);
        return -1;
    }

    //opened file: counting lines:
    int numBytesRead = 0;
    int totalReadInBytes = 0;
    char buffer[2];
    memset(buffer, '\0', 2*sizeof(char));
    do{
        numBytesRead = read(file, buffer, 1*sizeof(char));
        //printf("Buffer is: %s\n", buffer);
        //printf("Read bytes is: %d\n", numBytesRead);
        totalReadInBytes+=numBytesRead;
        if(numBytesRead < 0){
            printf("Fatal Error (bytes): %s\n", strerror(errno));
            int fileclose = close(file);
            if(fileclose < 0){
                printf("Fatal Error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            exit(EXIT_FAILURE);
        }
        else if(numBytesRead == 0 && totalReadInBytes == 0){ // empty file
            printf("[getNumLines] Empty file! %s\n", strerror(errno));
            return 0;
        }
        else if(buffer[0] == '\n'){
            linesRead = linesRead + 1;
            memset(buffer, '\0', 2*sizeof(char));
        }
    } while(numBytesRead != 0);

    if(linesRead == 0 && totalReadInBytes>0){ //no newline chars but has chars -> one line
        return 1;
    }

    return linesRead;
}

int existsFileManifest(char* manifestPath, char* filepath){
    if(getFileLineManifest(manifestPath, filepath, "-ps") == NULL){
        return 0;
    }
    return 1;
}
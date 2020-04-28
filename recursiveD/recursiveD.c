#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>

void recursivePrintDir(char*);
char* generateNewDirPath(char*, char*);
int compareString(char*, char*);
char* prependToStr(char*, char*);
char* appendToStr(char*, char*);
char* mallocStr(int);

int main(int argc, char** argv){
    
    recursivePrintDir("./");
    return 0;
}

int compareString(char* A, char* B){ //Simple str comparator
	int i = 0;
	while(A[i] != '\0'){
		if(B[i] == '\0'){
			return 1;
		}
		if(A[i] < B[i]){
			return -1;
		}
		if(A[i] > B[i]){
			return 1;
		}
		i++;
	}
	if(B[i] == '\0'){
		return 0;
	}
	return -1;
}

char* mallocStr(int sizeOfStr){ //Mallocs char* and automatically checks for errors
	char* str = (char*) malloc(sizeOfStr*sizeof(char));
	if(str == NULL){//Failed to malloc
		printf("Error: %s\n", strerror(errno));
		while(str == NULL){
            str = (char*) malloc(sizeOfStr*sizeof(char));
        }
	}
	return str;
}

char* prependToStr(char* oldStr, char* prefix){ //Prepends a str with a prefix str and returns the result as a newly malloced char*
	int oldLen = strlen(oldStr);
	int prefixLen = strlen(prefix);
	if(oldLen == 0 || prefixLen == 0){
		if(oldLen == 0){
			return prefix;
		}
		else if(prefixLen == 0){
			return oldStr;
		}
		return NULL;
	}
	char* finalStr = mallocStr( oldLen + prefixLen + 1);
	memcpy(finalStr, prefix, prefixLen*sizeof(char));
	memcpy(finalStr + prefixLen, oldStr, oldLen*sizeof(char));
	memset(finalStr + prefixLen + oldLen, '\0', 1*sizeof(char));
	return finalStr;
}

char* appendToStr(char* oldStr, char* suffix){ //Appends a suffix str to a str and returns the result as a newly malloced char*
	int oldLen = strlen(oldStr);
	int suffixLen = strlen(suffix);
	if(oldLen == 0 || suffixLen == 0){
		if(oldLen == 0){
			return suffix;
		}
		else if(suffixLen == 0){
			return oldStr;
		}
		return NULL;
	}
	char* finalStr = mallocStr(oldLen + suffixLen + 1);
	memcpy(finalStr, oldStr, oldLen*sizeof(char));
	memcpy(finalStr + oldLen, suffix, suffixLen*sizeof(char));
	memset(finalStr + oldLen + suffixLen, '\0', 1*sizeof(char));
	return finalStr;
}

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
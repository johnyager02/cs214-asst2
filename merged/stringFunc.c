#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

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

char* reallocStr(char* oldStr, int newSize){ //Reallocs char* and checks for errors
	char* newStr = (char*) realloc( oldStr, newSize*sizeof(char));
	if(newStr == NULL){//Failed to realloc
		printf("Error: %s\n", strerror(errno));
		while(newStr == NULL){
			newStr = (char*) realloc(oldStr, newSize*sizeof(char));
		}
	}
	return newStr;
}

char* trimEndToDelim(char* str, char delimiter){ // given "proj0/sub1/sub2/file1" -> gets "proj0/sub1/sub2/file1"
    int lastCharIndex;
	if(strlen(str) == 0){
		return NULL;
	}
	for(lastCharIndex = strlen(str) -1; lastCharIndex>=0;lastCharIndex--){
		if(str[lastCharIndex] == delimiter){
			char* final = mallocStr(strlen(str) + 1);
			bzero(final, strlen(str) + 1);
			memcpy(final, str, (lastCharIndex) * sizeof(char));
			//free(str);
			return final;
		}
	}
	return NULL;
}

int strToNum(char* str){
	int result;
	sscanf(str, "%d", &result);
	return result;
}
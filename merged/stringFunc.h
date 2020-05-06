#ifndef stringFunc
#define stringFunc

int compareString(char*, char*);
char* prependToStr(char*, char*);
char* appendToStr(char*, char*);
char* mallocStr(int);
char* reallocStr(char*, int);
char* trimEndToDelim(char*, char );
int strToNum(char*);

#endif
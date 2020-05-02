#ifndef manifestFunc
#define manifestFunc

unsigned char* getHash(char*);
unsigned char* mallocUChar(int);
void printHash(unsigned char*, char*);
unsigned char* mallocUChar(int);
void printHashR(char*);
char* getLineToAdd(int, char*);
void addToManifest(char*, char*);
void initializeManifest(char*);
char* getFileLineManifest(char*, char*, char*);
char* getHashStrLine(char*);
char* hashToStr(unsigned char*);
void modifyManifest(char*, int, char*, char*);
char* numToStr(int);
char* getFilePathStrLine(char*);
//void writeAfterChar(char*, int, char*, char, int);
void removeLine(char*, int);
char* getFileContents(char*);
char* getProjVersion(char*);
char* getLineFile(char*, int);
void setLineFile(char*, int, char*);
char* nthToken(char*, int, char);
int getNumLines(char*);

#endif
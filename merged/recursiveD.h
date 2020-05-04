#ifndef recursiveD
#define recursiveD

void recursivePrintDir(char*);
char* generateNewDirPath(char*, char*);
char* generateNewFilePath(char*, char*);
void recursiveDelete(char*);
int isEmptyDir(char*);
void createNewFile(char* );
void recursiveMakeSubDir(char*);
int existsFile(char*);
int existsDir(char* );

#endif
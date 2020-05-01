#ifndef WTF
#define WTF

int existsFile(char*);
void configure(char*, char*);
void checkout(char* );
void update(char* );
void upgrade(char* );
void commit(char* );
void push(char* e);
void create(char* );
void destroy(char* );
void add(char* , char* );
void removeEntry(char* , char* );
void currentversion(char* );
void history(char* );
void rollback(char* , int );
char* nthDirPathToName(char*, int);
char* filepathToName(char*);
int existsFile(char*);


#endif
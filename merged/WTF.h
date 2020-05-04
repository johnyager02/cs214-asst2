#ifndef WTF
#define WTF


void configure(char*, char*);
void checkout(char*, int);
void update(char*, int );
void upgrade(char* ,int);
void commit(char* , int);
void push(char* , int);
void create(char* , int);
void destroy(char* , int);
void add(char* , char* );
void removeEntry(char* , char* );
void currentversion(char*, int );
void history(char* , int);
void rollback(char* ,int, int );
char* nthDirPathToName(char*, int);
char* filepathToName(char*);



#endif
#ifndef sendAndReceive
#define sendAndReceive

void sendData(int, char* , char* );
void fetchData(int, char*, char*);
void sendCommand(int, char*, char*);
char** getOutputArrSent(char, char , char* , char* , char* );
char** getOutputArrFetched(char , char , char* , char* );
char* readFromSock(int, int);
char* readFromSockIntoBuff(int, char*, int);
int writeToSock(int, char*);
char* getNextUnknownLen(int);

#endif
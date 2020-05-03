#ifndef sendAndReceive
#define sendAndReceive

char** readInput(char*, int);
void sendData(int, char* , char* );
void fetchData(int, char*, char*);
void sendCommand(int, char*, char*);
char** getOutputArrSent(char, char , char* , char* , char* );
char** getOutputArrFetched(char , char , char* , char* );

#endif
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>

void recursivePrintDir(char*);

int main(int argc, char** argv){
    

    return 0;
}

void recursivePrintDir(char* filepath){
    DIR* currentDir = opendir(filepath);
}
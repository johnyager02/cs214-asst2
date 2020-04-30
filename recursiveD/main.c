#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<dirent.h>
#include"stringFunc.h"
#include"recursiveD.h"
#include"manifestFunc.h"
int main(int argc, char** argv){
    


    //recursivePrintDir("./");
    //printHashR("./");

    //initializeManifest("./proj0");
    //char* line = getLineToAdd(0, "./proj0/test0");
    //addToManifest("./proj0/.Manifest", line);

    //testing modifyManifest(char* <manifestPath>, int <lineNumToChange>, char* <changeFlag>, char* <replacementStr> ); 
    //argument0: path to .Manifest... "./proj0/.Manifest"
    //argument1: line number of change... line 1 is the first line containing project version
    //argument2: flagChange: -v for changing version, -p for changing filepath, -h for changing hash, -c for changing serverCheckChar
    //argument3: actualChange: new Str to write to .Manifest file
    // modifyManifest("./proj0/.Manifest", 1, "-v", numToStr(1));
    // modifyManifest("./proj0/.Manifest", 1, "-p", "./proj0/test1");
    // modifyManifest("./proj0/.Manifest", 1, "-h", "2a4f5a0b4554c6dd26a995dcec265b05b52eba06");
    // modifyManifest("./proj0/.Manifest", 1, "-c", "y");

    //testing getFileLineManifest(), w/ -hs / -hi; -hs is search by hash -> returns line as a str, -hi is search by hash -> returns lineNum
    // char* currentLine = getFileLineManifest("./proj0/.Manifest", "./proj0/test0", "-hi");
    // if(currentLine != NULL){
    //     printf("[Main] currentLine is: \"%s\"\n", currentLine);
    // }
    // else if(currentLine == NULL){
    //     printf("[Main] error!\n");
    // }

    //testing getFileLineManifest(), w/ -ps / -pi; -ps is search by filepath -> returns line as a str, -hi is search by filepath -> returns lineNum
    char* currentLine = getFileLineManifest("./proj0/.Manifest", "./proj0/test1", "-pi");
    if(currentLine != NULL){
        printf("[Main] currentLine is: \"%s\"\n", currentLine);
    }
    else if(currentLine == NULL){
        printf("[Main] error!\n");
    }

    //testing getFilePathStrLine()...
    // char* filepath = getFilePathStrLine("0 ./proj0/test0 0071877d20a65c02d9a1654f109b97dc61416d1a n\n");
    // printf("[Main] filepath is: \"%s\"\n", filepath);
    
    return 0;
}
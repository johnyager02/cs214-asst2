#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<dirent.h>
#include"stringFunc.h"
#include"recursiveD.h"
#include"manifestFunc.h"
#include"WTF.h"

int main(int argc, char** argv){
    


    //recursivePrintDir("./");
    //printHashR("./");

    //initializeManifest("./proj0");
    // char* line = getLineToAdd(0, "./proj0/test0");
    // addToManifest("./proj0/.Manifest", line);
    // line = getLineToAdd(0, "./proj0/test1");
    // addToManifest("./proj0/.Manifest", line);
    // line = getLineToAdd(0, "./proj0/test2");
    // addToManifest("./proj0/.Manifest", line);

    //testing modifyManifest(char* <manifestPath>, int <lineNumToChange>, char* <changeFlag>, char* <replacementStr> ); 
    //argument0: path to .Manifest... "./proj0/.Manifest"
    //argument1: line number of change... line 0 is the first line containing only project version, line 1 is first fileLine
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
    // char* currentLine = getFileLineManifest("./proj0/.Manifest", "./proj0/test1", "-pi");
    // if(currentLine != NULL){
    //     printf("[Main] currentLine is: \"%s\"\n", currentLine);
    // }
    // else if(currentLine == NULL){
    //     printf("[Main] error!\n");
    // }

    //testing getFilePathStrLine()...
    // char* filepath = getFilePathStrLine("0 ./proj0/test0 0071877d20a65c02d9a1654f109b97dc61416d1a n\n");
    // printf("[Main] filepath is: \"%s\"\n", filepath);
    
    //testing writeAfterChar(char* filepath, char* lineNum, char* change, char charToSkip, int numSkipsToDo)
    // writeAfterChar("./proj0/.Manifest", 1, numToStr(0), ' ', 0);
    // writeAfterChar("./proj0/.Manifest", 1, "./proj0/test0", ' ', 1);
    // writeAfterChar("./proj0/.Manifest", 1, "0071877d20a65c02d9a1654f109b97dc61416d1a", ' ', 2);
    // writeAfterChar("./proj0/.Manifest", 1, "n", ' ', 3);

    // writeAfterChar("./proj0/.Manifest", 2, numToStr(0), ' ', 0);
    // writeAfterChar("./proj0/.Manifest", 2, "./proj0/test1", ' ', 1);
    // writeAfterChar("./proj0/.Manifest", 2, "2a4f5a0b4554c6dd26a995dcec265b05b52eba06", ' ', 2);
    // writeAfterChar("./proj0/.Manifest", 2, "n", ' ', 3);

    //test removeLine()
    //removeLine("./proj0/.Manifest", 1);


    //test filepathToName() -> works for correct input!
    //printf("[Main] filename is: \"%s\"\n", filepathToName("./proj0/dir0/test0"));

    //test nthDirPathToName() -> works for correct input!
    //printf("[Main] \"%dth\" dirname is: \"%s\"\n", 2, nthDirPathToName("./proj0/dir0/", 2));
    
    //test existsFile() First format; "Makefile"
    // if(existsFile("Makefile") == 1){
    //     printf("[Main] File: \"%s\" exists!\n", "Makefile");
    // }else{
    //     printf("[Main] File: \"%s\" does NOT exist!\n", "Makefile");
    // }
    
    // //test existsFile() Second format; "proj0/test0"
    // if(existsFile("proj0/test0") == 1){
    //     printf("[Main] File: \"%s\" exists!\n", "proj0/test0");
    // }else{
    //     printf("[Main] File: \"%s\" does NOT exist!\n", "proj0/test0");
    // }

    //test add()
    // add("proj0", "proj0/test0");
    // add("proj0", "proj0/test1");
    // add("proj0", "proj0/test2");
    // add("proj0", "Makefile");
    // add("proj0", "proj0/subproj0/subtest0");
    // add("proj1", "proj0/subproj0/subtest0");
    
    //test removeEntry()
    // removeEntry("proj0", "proj0/test0");
    // removeEntry("proj0", "proj0/test1");
    // removeEntry("proj0", "proj0/test2");
    // removeEntry("proj0", "Makefile");
    // removeEntry("proj0", "proj0/subproj0/subtest0");
    // removeEntry("proj1", "proj0/subproj0/subtest0");

    //test currentVersion()
    //currentversion("proj0");
    return 0;
}
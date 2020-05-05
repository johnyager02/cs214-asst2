#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<dirent.h>
#include"stringFunc.h"
#include"recursiveD.h"
#include"manifestFunc.h"
#include"WTF.h"
#include"sendAndReceive.h"

int main(int argc, char** argv){
    

    //Test recursivePrintDir(filpath); ->Works!
    //recursivePrintDir("./"); 
    
    //Test printHashR(filepath); ->Works with / at the end!
    //printHashR("proj0/");

    //Test initializeManifest(project)
    //initializeManifest("proj0"); //-> Works!
    //initializeManifest("./proj0"); -> Works!

    //Test addToManifest() -> works for all cases so far!
    // char* line = getLineToAdd(0, "proj0/test0");
    // addToManifest("proj0/.Manifest", line);
    // line = getLineToAdd(0, "proj0/test1");
    // addToManifest("proj0/.Manifest", line);
    // line = getLineToAdd(0, "proj0/test2");
    // addToManifest("proj0/.Manifest", line);
    // line = getLineToAdd(0, "Makefile");
    // addToManifest("proj0/.Manifest", line);

    //testing modifyManifest(char* <projname>, int <lineNumToChange>, char* <changeFlag>, char* <replacementStr> ); 
    //argument0: path to .Manifest... "./proj0/.Manifest"
    //argument1: line number of change... line 0 is the first line containing only project version, line 1 is first fileLine
    //argument2: flagChange: -v for changing version, -p for changing filepath, -h for changing hash, -c for changing serverCheckChar
    //argument3: actualChange: new Str to write to .Manifest file
    // char* versionnum =numToStr(12);
    // modifyManifest("proj0", 1, "-v", versionnum);
    // free(versionnum);
    // modifyManifest("proj0", 1, "-p", "Makefile");
    // modifyManifest("proj0", 1, "-h", "853305468f15be1ef380422489554b7e5694ebca");
    // modifyManifest("proj0", 1, "-c", "y");

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

    //test getFileContents(char* filepath)
    //getFileContents("proj0/test1");

    //test sendAndReceive functions:
    // readInput("ss8:project18:thisfile20:thfufjdkslfjiweoiwfjjdskfl",20);
    // send(1, "thisProject", "proj0/test1");
    // fetch(31, "newProject", ".Manifest");

    //Test getProjVersion(projname)
    //printf("[Main]-Project version is: \"%s\"\n", getProjVersion("proj0"));

    //Test setProjVersion(projname) ->works!
    //setProjVersion("proj0", "12\n");

    //Test getLineFile()
    // printf("[Main] line at index %d is: \"%s\"\n", 0, getLineFile("proj0/.Manifest", 0));
    // printf("[Main] line at index %d is: \"%s\"\n", 1, getLineFile("proj0/.Manifest", 1));
    // printf("[Main] line at index %d is: \"%s\"\n", 2, getLineFile("proj0/.Manifest", 2));
    // printf("[Main] line at index %d is: \"%s\"\n", 3, getLineFile("proj0/.Manifest", 3));
    // printf("[Main] line at index %d is: \"%s\"\n", 4, getLineFile("proj0/.Manifest", 4));

    //Test nthToken();
    // printf("[Main] %dth token is: \"%s\"\n", 0, nthToken("11 Makefile 8533054 n\n", 0, ' '));
    // printf("[Main] %dth token is: \"%s\"\n", 1, nthToken("11 Makefile 8533054 n\n", 1, ' '));
    // printf("[Main] %dth token is: \"%s\"\n", 2, nthToken("11 Makefile 8533054 n\n", 2, ' '));
    // printf("[Main] %dth token is: \"%s\"\n", 3, nthToken("11 Makefile 8533054 n\n", 3, ' '));
    // printf("[Main] %dth token is: \"%s\"\n", 4, nthToken("11 Makefile 8533054 n\n", 4, ' '));

    //Test getNumLines()
    //printf("[Main] Number of lines in file: \"%s\" is %d\n", "proj0/.Manifest", getNumLines("proj0/.Manifest"));
    //printf("[Main] Number of lines in file: \"%s\" is %d\n", "proj0/.Manifest", getNumLines("proj0/.Manifest"));

    //Test create()
    //create("proj0", 1234);

    //Test recursiveDelete() -> works only with / at the end of initial Dirpath
    //recursiveDelete("testDelete/");

    //Test isEmptyDir() -> Works!
    // if(isEmptyDir("testDelete")){
    //     printf("IS EMPTY\n");
    // }else printf("NOT EMPTY\n");

    //Test writing file
    // int fd = creat("proj1/test1", O_RDWR);
    // if(fd<0){
    //     printf("Create failed:\n");
    // }
    //Test trimend
    // printf("[Main] str after trim is: \"%s\"\n", trimEndToDelim("proj0/sub1/sub2/file1", '/')); -> "proj0/sub1/sub2"
    // printf("[Main] str after trim is: \"%s\"\n", trimEndToDelim("proj0/sub1/sub2", '/')); -> "proj0/sub1"
    // printf("[Main] str after trim is: \"%s\"\n", trimEndToDelim("proj0/sub1", '/')); -> "proj0"
    // printf("[Main] str after trim is: \"%s\"\n", trimEndToDelim("proj0", '/')); -> NULL
    //Test writeNewFile()
    //writeNewFile("proj1/sub1/sub2/sub3/file1"); -> works!
    //recursiveDelete("proj1/");
    
    // char* line = getLineToAdd(0, "proj0/test0");
    // addToManifest("server/proj1/.Manifest", line);
    // line = getLineToAdd(0, "proj0/test1");
    // addToManifest("server/proj1/.Manifest", line);
    // line = getLineToAdd(0, "proj0/test2");
    // addToManifest("server/proj1/.Manifest", line);
    // line = getLineToAdd(0, "Makefile");
    // addToManifest("server/proj1/.Manifest", line);

    

    //Test void checkout() -> works clientside!!
    //Resetting test files: create files
    // createNewFile("server/proj1/test0");
    // createNewFile("server/proj1/test1");
    // createNewFile("server/proj1/test2");
    // overwriteOrCreateFile("server/proj1/test0", "test0succeeded");
    // overwriteOrCreateFile("server/proj1/test1", "test1succeeded");
    // overwriteOrCreateFile("server/proj1/test2", "test2succeeded");
    //  recursiveDelete("client/proj1/");
    //  rmdir("client/proj1/");
    //checkout("proj1", 11);
    
    //Test void update() -> works for full success case and A, need to test M and C!
    //Resetting test files:
    //remove("proj1/.Update");
    //update("proj1", 11);

    return 0;
}
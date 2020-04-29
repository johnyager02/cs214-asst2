#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include"stringFunc.h"
#include"recursiveD.h"
#include"manifestFunc.h"
int main(int argc, char** argv){
    


    //recursivePrintDir("./");
    //getHash("./sha1Main1");
   //getHash("./sha1Main2");
    //printHashR("./");
    initializeManifest("./proj0");
    char* line = getLineToAdd(0, "./proj0/test0");
    addToManifest("./proj0/.Manifest", line);
    return 0;
}
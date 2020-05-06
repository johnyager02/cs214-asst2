#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<errno.h>
#include<openssl/sha.h>
#include"stringFunc.h"
#include"recursiveD.h"
#include"manifestFunc.h"
#include"sendAndReceive.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<unistd.h>
#define BUFFSIZE 10


void handleServerFetched(char** output, int clientSockFd){
    char* status = output[0];
    char* commandType = output[1];
    char* projName = output[2];
    char* fileName = output[3];
    
    if(existsDir(projName) != 1){
        //Failed to find requested project
        //send failed
        sendData(clientSockFd, projName, "");
    }
    if(existsFile(fileName) != 1){
        //Failed to find requested file
        //send failed
        sendData(clientSockFd, projName, "");
    }

    //Client fetched and file/project both exist -> send file that is requested
    sendData(clientSockFd, projName, fileName);

    //Free memory:

}

void handleServerSent(char** output, int clientSockFd){

}

char** readInputFromServer(int sockfd){
    //Want to read <s/f><s/f/c><projLen>:<projName><fileLen>:<fileName> or for send:<s/f><s/f/c><projLen>:<projName><fileLen>:<fileName><dataLen>:<data>
    //Read successflag and commandType
    char* firstRead = readFromSock(sockfd, 2*sizeof(char));
    if(firstRead==NULL){
        //Failed
        printf("[readInputProtocol] Error: firstRead NULL\n");
        return NULL;
    }
    if(strlen(firstRead) != 2){
        //Failed
        printf("[readInputProtocol] Error: firstRead did not read 2 bytes\n");
        return NULL;
    }
    //Handle success or fail
    char success = firstRead[0];
    if(success != 'f' && success != 's'){
        printf("[readInputProtocol] Unknown message\n");
        return NULL;
    }
    if(success == 'f'){
        printf("[readInputProtocol] Error! command could not be executed\n");
        return NULL;
    }
    //Parse commandType
    char commandType = firstRead[1];

    //Read project length
    char* projectLengthString = getNextUnknownLen(sockfd);
    
    int projectLength;
    sscanf(projectLengthString, "%d", &projectLength);
    
    //Read projName
    char* projectName = mallocStr(projectLength+1);
    bzero(projectName, (projectLength+1)*sizeof(char));
    projectName = readFromSockIntoBuff(sockfd, projectName, projectLength);

    //Read fileName Length:
    char* fileLengthString = getNextUnknownLen(sockfd);
    int fileLength;
    sscanf(fileLengthString, "%d", &fileLength);
    
    //Read fileName:
    char* fileName = mallocStr(fileLength+1);
    bzero(fileName, (fileLength+1)*sizeof(char));
    fileName = readFromSockIntoBuff(sockfd, fileName, fileLength);

    //Done reading...
    printf("[readInputProtocol] Done Reading -> handling commandType cases\n");
    
    
    if(commandType == 's'){
        //Read dataLen:
        char* dataLengthString = getNextUnknownLen(sockfd);
        printf("[readInputProtocol] DataLen is: \"%s\"\n", dataLengthString);
        int dataLength;
        sscanf(dataLengthString, "%d", &dataLength);
        
        //Read data:
        char* data = mallocStr(dataLength+1);
        bzero(data, (dataLength+1)*sizeof(char));
        data = readFromSockIntoBuff(sockfd, data, dataLength);

        printf("[readInputProtocol] %c %c %s %s %d %s\n", success, commandType, projectName, fileName, dataLength, data);

        //handleSend
        return (char**) getOutputArrSent(success, commandType, projectName, fileName, data);
    }
    else if(commandType == 'f'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleFetch
        char** output = (char**) getOutputArrFetched(success, commandType, projectName, fileName);
        handleServerFetched(output, sockfd);
        return output;
    }
    else if(commandType == 'c'){
        printf("[readInput] %c %c %s %s\n", success, commandType, projectName, fileName);
        //handleSendCommand
        //return (char**) getOutputArrFetched(success, commandType, projectName, fileName);
    }
    else{
        printf("Error: command type not recognized");
        return NULL;
    }
    return NULL;
}

//NOTE! format of names in arguments for filenames/dirnames: "file1" || "sub0/subsub92/file716"


char* filepathToName(char* filepath){ //given filepath -> return filename EX) "./proj0/test0" -> "test0"
    char* filename = (char*) mallocStr(strlen(filepath));
    memset(filename, '\0', strlen(filepath)*sizeof(char));

    //
    int i;
    int indexOfCopy = 0;
    for(i = 0; i<strlen(filepath);i++){
        if(filepath[i] == '/'){//reset buffer
            memset(filename, '\0', strlen(filepath)*sizeof(char));
            indexOfCopy = 0;
        }
        else{
            memcpy(filename + indexOfCopy, filepath + i, 1*sizeof(char));
            indexOfCopy++;
        }
    }
    return filename;
}

char* nthDirPathToName(char* dirpath, int n){ //given dirpath -> return nth dirname EX) "./proj0/dir0/" w/ n=2-> "dir0"; n is like array index, n=0 is first dirname "."
    char* dirname = (char*) mallocStr(strlen(dirpath));
    memset(dirname, '\0', strlen(dirpath)*sizeof(char));

    //
    int i;
    int indexOfCopy = 0;
    int numSlash = 0;
    for(i = 0; i<strlen(dirpath);i++){
        if(dirpath[i] == '/'){//reset buffer
            if(numSlash == n){
                return dirname;
            }
            memset(dirname, '\0', strlen(dirpath)*sizeof(char));
            indexOfCopy = 0;
            numSlash++;
        }
        else{
            memcpy(dirname + indexOfCopy, dirpath + i, 1*sizeof(char));
            indexOfCopy++;
        }
    }
    return NULL;
}


void configure(char* hostname, char* port){
    //create ./configure file that contains IP and port 
}

void checkout(char* projname, int sockfd){
/*If it does run it will request the entire project from the server, which will send over the current version of the
project .Manifest as well as all the files that are listed in it. The client will be responsible for receiving the
project, creating any subdirectories under the project and putting all files in to place as well as saving the
.Manifest. */
   
    char* manifestPath = appendToStr(projname, "/.Manifest");
    
    //fetch server's .Manifest for the given project
    fetchData(sockfd, projname, manifestPath);
    char** output =  readInputFromServer(sockfd);

    if(output == NULL){
        printf("[checkout] Failed!\n");
        //close(sockfd);
        return;
    }
    if(output[0][0] == 'f'){
        printf("[checkout] Failed!\n");
        //close(sockfd);
        return;
    }
    char* serverFileData = output[4];
    printf("[checkout] serverManifest is: \"%s\"\n", serverFileData);

    //checks if project exists -> if not make
    if(existsDir(projname) != 1){
        int mkProj = mkdir(projname, 0777);
    }
    //proj0/sub1/sub2/sub3/text.txt
    //create manifest
    createNewFile(manifestPath);
    //setFileContents to manifest received from server;
    overwriteOrCreateFile(manifestPath, serverFileData);

    //TESTING DATA: -> Comment out after getting server receive to work!
    // char* testServerManifestData = getFileContents("server/proj1/.Manifest");
    // char* testServerFile1 = getFileContents("server/proj1/test0");
    // char* testServerFile2 = getFileContents("server/proj1/test1");
    // char* testServerFile3 = getFileContents("server/proj1/test2");
    //overwriteOrCreateFile(manifestPath, testServerManifestData);

    //write all files to their locations
    int numLinesManifest = getNumLines(manifestPath);
    
    int i;
    for(i = 1; i<numLinesManifest;i++){ // for each file in manifest fetch and place into correct place
        char* fetchLine = getLineFile(manifestPath, i);
        char* fetchFileName = nthToken(fetchLine, 1, ' '); //get filename from manifest
        printf("[checkout] Filename of line %d to fetch is: \"%s\"\n", i, fetchFileName); 
        //fetch file from server
        fetchData(sockfd, projname, fetchFileName);
        //receive file from server
        output = readInputFromServer(sockfd);
        serverFileData = output[4];
        //create manifest
        createNewFile(fetchFileName);
        //setFileContents to manifest received from server;
        overwriteOrCreateFile(fetchFileName, serverFileData);
    }
    //sendStop fetching
    //sendCommand(sockfd, projname, "stop");
    printf("[checkout] Checkout for project: \"%s\" succeeded!\n", projname);
    // createNewFile("proj1/test0");
    // createNewFile("proj1/test1");
    // createNewFile("proj1/test2");
    // overwriteOrCreateFile("proj1/test0", testServerFile1);
    // overwriteOrCreateFile("proj1/test1", testServerFile2);
    // overwriteOrCreateFile("proj1/test2", testServerFile3);
}

char* allocateCommitLine(char updateChar, char* filename, char* hash, int newVersion){
    char* newVersionStr = numToStr(newVersion);
    char* lineToAppend = mallocStr(1 + 1 + strlen(filename) + 1 + strlen(hash) + 1 + strlen(newVersionStr) + 1 + 1);
    bzero(lineToAppend, (1 + 1 + strlen(filename) + 1 + strlen(hash) + 1 + strlen(newVersionStr) + 1 + 1)*sizeof(char));
    sprintf(lineToAppend, "%c %s %s %s\n", updateChar, filename, hash, newVersionStr);
    printf("[allocateCommitLine] %c-final string to append is: \"%s\"\n", updateChar, lineToAppend);
    return lineToAppend;
}

char* allocateUpdateLine(char updateChar, char* filename, char* hash){
    char* lineToAppend = mallocStr(1 + 1 + strlen(filename) + 1 + strlen(hash) + 1 + 1);
    bzero(lineToAppend, (1 + 1 + strlen(filename) + 1 + strlen(hash) + 1 + 1)*sizeof(char));
    sprintf(lineToAppend, "%c %s %s\n", updateChar, filename, hash);
    printf("[handCaseUpdateMC] %c-final string to append is: \"%s\"\n", updateChar, lineToAppend);
    return lineToAppend;
}

char* handleCaseUpdateMC(char* currentClientLine, char* currentServerLine, char* currentServerFileName){ // Handle M or C for update()
    //Get storedClientVersion
    char* clientFileVersion = nthToken(currentClientLine, 0, ' ');
    int clientFileVersionInt;
    sscanf(clientFileVersion, "%d", &clientFileVersionInt);

    //Get storedServerVersion
    char* serverFileVersion = nthToken(currentServerLine, 0, ' ');
    int serverFileVersionInt;
    sscanf(serverFileVersion, "%d", &serverFileVersionInt);

    //Get stored client hash
    char* storedClientHash = nthToken(currentClientLine, 2, ' ');
    printf("[handleCaseUpdateMC] storedClientHash is: \"%s\"\n", storedClientHash);
    
    //Get stored server hash
    char* storedServerHash = nthToken(currentServerLine, 2, ' ');
    printf("[handleCaseUpdateMC] storedServerHash is: \"%s\"\n", storedServerHash);
    
    //Get live client hash
    char* liveClientHash = hashToStr(getHash(currentServerFileName));
    printf("[handleCaseUpdateMC] liveClientHash is: \"%s\"\n", liveClientHash);
    
    //Compare stored hashes   
    if(clientFileVersionInt!=serverFileVersionInt && compareString(storedClientHash, storedServerHash) != 0 && compareString(storedClientHash, liveClientHash)==0){ // M case
                                      //"M <filename> <serverfilehash>\n" -> client has to get updated version of the file (M)
        char* lineToAppend = allocateUpdateLine('M', currentServerFileName, storedServerHash);
        return lineToAppend;
    } 
    else if(compareString(liveClientHash, storedClientHash) != 0 && compareString(storedClientHash, storedServerHash) != 0){ // (C) case1 local change
                                      //"M <filename> <serverfilehash>\n" -> client made a local change -> msut be resolved before updating
        char* lineToAppend = allocateUpdateLine('C', currentServerFileName, liveClientHash);
        return lineToAppend;
    }
    else if(clientFileVersionInt == serverFileVersionInt && compareString(storedClientHash, storedServerHash) != 0){ // (C) case 2 same version, different hashes -> conflict 
        char* lineToAppend = allocateUpdateLine('C', currentServerFileName, liveClientHash);
        return lineToAppend;
    }
    else if(clientFileVersionInt!= serverFileVersionInt && compareString(storedClientHash, storedServerHash) == 0){ // (C) case 3 different versions, same hashes -> conflict
        char* lineToAppend = allocateUpdateLine('C', currentServerFileName, liveClientHash);
        return lineToAppend;
    }
    return NULL;
}

void add(char* projname, char* filename, int fileversion){ // Expects projname as format: "proj0" and filename format as: "test0" && "proj0/test0"
    if(existsDir(projname) != 1){ //Checks if project exists
        //Failed
        printf("[add] ERROR! This proj: \"%s\" does not exist in the client's machine\n", projname);
        return;
    }
    char* manifestPath = appendToStr(projname, "/.Manifest");
    if(getFileLineManifest(manifestPath, filename, "-ps")!=NULL){//file already in manifest
        printf("[add] ERROR! This file: \"%s\" already exists in the Manifest\n", filename);
        return;
    } 
    //File exists and is not already in manifest -> append to .Manifest
    printf("[add] Adding file: \"%s\" to the manifest of project: \"%s\"\n", filename, projname);
    char* lineToAdd = getLineToAdd(fileversion, filename);
    addToManifest(manifestPath, lineToAdd);
}

void removeEntry(char* projname, char* filename){ // Expects projname as format: "proj0" and filename format as: "test0" && "proj0/test0"
    if(existsDir(projname) != 1){ //Checks if project exists
        //Failed
        printf("[removeEntry] ERROR! This proj: \"%s\" does not exist in the client's machine\n", projname);
        return;
    }
    char* manifestPath = appendToStr(projname, "/.Manifest");
    char* lineNumStr = getFileLineManifest(manifestPath, filename, "-pi");
    int lineNumToDelete;
    if(lineNumStr == NULL){
        lineNumToDelete = -1;
    }
    else{
        lineNumToDelete;
        sscanf(lineNumStr, "%d", &lineNumToDelete);
    }
    if(lineNumToDelete == -1){//file to be deleted is NOT in manifest
        printf("[removeEntry] ERROR! This file: \"%s\" does NOT exist in the Manifest\n", filename);
        return;
    } 
    //File exists and is in the manifest -> delete entry
    printf("[removeEntry] Deleting file: \"%s\" from the manifest of project: \"%s\"\n", filename, projname);
    removeLine(manifestPath, lineNumToDelete);
}

void update(char* projname, int sockfd){ 
    //Fetch server's manifest
    char* manifestPath = appendToStr(projname, "/.Manifest");
    
    
    fetchData(sockfd, projname, manifestPath);
    char** output = readInputFromServer(sockfd);
    //receive manifest into char** output output[0] == status, output[1] == commandType, output[2] == projName, output[3] = fileName, output[4] = filedata; 
    if(output == NULL){
        printf("[update] Failed!\n");
        //close(sockfd);
        return;
    }
    if(output[0][0] == 'f'){
        printf("[update] Failed!\n");
        //close(sockfd);
        return;
    }
    char* serverFileData = output[4];
    printf("[update] serverManifest is: \"%s\"\n", serverFileData);
    
    //TESTING:
    //char* serverManifestData = getFileContents("server/proj1/.Manifest");

    //Initiate temp server manifest
    char* tempServerManifest = appendToStr(projname, "/.serverManifest");
    createNewFile(tempServerManifest);
    overwriteOrCreateFile(tempServerManifest, serverFileData);

    //Create empty .Update file
    char* updatePath = appendToStr(projname, "/.Update");
    createNewFile(updatePath);
    open(updatePath, O_RDONLY|O_TRUNC, 00644);
    
    //Compare .Manifest versions
    if(compareString(getLineFile(tempServerManifest, 0), getLineFile(manifestPath, 0)) == 0){//Manifests are same version //Full success
        char* conflictPath = appendToStr(projname, "/.Conflict");
        if(existsFile(conflictPath) == 1){
            remove(conflictPath);
        }
        printf("Up To Date");
        remove(tempServerManifest);
        return;
    }
    
    //Manifest versions different -> search for difference 
    char* conflictPath;
    char* currentClientLine;
    char* currentServerLine;
    int conflictFound = 0;
    //First search with serverManifest
    int numLinesServerManifest = getNumLines(tempServerManifest);
    int i;
    for(i = 1;i<numLinesServerManifest;i++){ //skip proj version
        currentServerLine = getLineFile(tempServerManifest, i);
        char* currentServerFileName = nthToken(currentServerLine, 1, ' ');
        char* currentServerFileHash = nthToken(currentServerLine, 2, ' ');
        printf("[update] currentServerFileName is: \"%s\"\n", currentServerFileName);
        printf("[update] currentServerFileHash is: \"%s\"\n", currentServerFileHash);
        if( (currentClientLine = getFileLineManifest(manifestPath, currentServerFileName, "-ps") ) != NULL){ //server's manifest file exists on client's manifest -> Check M or C
            printf("[update] Client manifest has file: \"%s\"\n", currentServerFileName);
            char* lineToAppend = handleCaseUpdateMC(currentClientLine, currentServerLine, currentServerFileName);
            if(lineToAppend == NULL){
                continue;
            }
            if(conflictFound == 0 && lineToAppend[0] == 'C'){ // first conflict -> create conflict file
                conflictFound = 1;
                //initialize .Conflict file
                conflictPath = appendToStr(projname, "/.Conflict");
                createNewFile(conflictPath);
                open(conflictPath, O_RDONLY|O_TRUNC, 00644);
                addToManifest(conflictPath, lineToAppend);
            }
            else if(conflictFound != 0 && lineToAppend[0] == 'C'){
                addToManifest(conflictPath, lineToAppend);
            }
            else if(lineToAppend[0] == 'M'){
                addToManifest(updatePath, lineToAppend);
            }
            free(lineToAppend);
            
        }
        else{ //server has files not on client's manifest -> Append A to .Update
            printf("[update] Server has file: \"%s\" which is not in client's manifest\n", currentServerFileName);
            //Format "<A> <filename>"
            char* lineToAppend = allocateUpdateLine('A', currentServerFileName, currentServerFileHash);
            addToManifest(updatePath, lineToAppend); //reuse addToManifest to append lineToAppend to .Update
            free(lineToAppend);
        }
    }

    //Second search with client manifest for files to Delete
    int numLinesClientManifest = getNumLines(manifestPath);
    int j;
    for(j = 1;j<numLinesClientManifest;j++){ //skip proj version
        currentClientLine = getLineFile(manifestPath, j);
        char* currentClientFileName = nthToken(currentClientLine, 1, ' ');
        char* currentClientFileHash = nthToken(currentClientLine, 2, ' ');
        printf("[update] currentClientFileName is: \"%s\"\n", currentClientFileName);
        printf("[update] currentClientFileHash is: \"%s\"\n", currentClientFileHash);
        if( (currentServerLine = getFileLineManifest(tempServerManifest, currentClientFileName, "-ps") ) == NULL){ //client's manifest file exists on server's manifest -> Check M or C
            printf("[update] Client has file: \"%s\" which is not in server's manifest\n", currentClientFileName);
            //Format "<D> <filename> <hash>\n"
            char* lineToAppend = allocateUpdateLine('D', currentClientFileName, currentClientFileHash);
            addToManifest(updatePath, lineToAppend); //reuse addToManifest to append lineToAppend to .Update
            free(lineToAppend);
        }
    }

    if(conflictFound == 1){
        //delete .Update file if there is a conflict
        remove(updatePath);
        printf("Conflicts were found for %s and must be resolved before updating!\n", projname);
    }
    remove(tempServerManifest);
}

void upgrade(char* projname, int sockfd){
    //Error checking 
    //1) No .Update on clientside
    char* updatePath = appendToStr(projname, "/.Update");
    char* conflictPath = appendToStr(projname, "/.Conflict");
    char* manifestPath = appendToStr(projname, "/.Manifest");
    if(existsFile(updatePath) != 1){
        printf("An update must be run before upgrading %s\n", projname);
        return;
    }
    else if(existsFile(updatePath) == 1 && isEmptyFile(updatePath) == 1){
        printf("Project %s is up to date\n");
        remove(updatePath);
        return;
    }
    //2) .Conflict file exists
    if(existsFile(conflictPath) == 1){
        printf("All conflicts must first be resolved and an update run on %s before upgrading\n", projname);
    }

    //fetch server's .Manifest to do file version updates ->
    fetchData(sockfd, projname, manifestPath);
    char** output = readInputFromServer(sockfd);
    //receive manifest into char** output output[0] == status, output[1] == commandType, output[2] == projName, output[3] = fileName, output[4] = filedata; 
    if(output == NULL){
        printf("[upgrade] Failed!\n");
        //close(sockfd);
        return;
    }
    if(output[0][0] == 'f'){
        printf("[upgrade] Failed!\n");
        //close(sockfd);
        return;
    }
    char* serverFileData = output[4];
    printf("[upgrade] serverManifest is: \"%s\"\n", serverFileData);

    //Initiate temp server manifest
    char* tempServerManifest = appendToStr(projname, "/.serverManifest");
    createNewFile(tempServerManifest);
    overwriteOrCreateFile(tempServerManifest, serverFileData);

    //handle non-empty .Update file
    int numLinesUpdate = getNumLines(updatePath);
    int i;
    char* currentUpdateLine;
    char* currentFileName;
    char* currentFileHash;
    char* currentServerFileLine;
    char* currentServerFileVersionStr;
    int currentServerFileVersion;
    char** fetchedFile;
    for(i = 0; i<numLinesUpdate;i++){
        currentUpdateLine = getLineFile(updatePath, i);
        currentFileName = nthToken(currentUpdateLine, 1, ' ');
        currentFileHash = nthToken(currentUpdateLine, 2, ' ');
        if(currentUpdateLine[0] == 'D'){
            //handle 'D' -> delete all files with this tag from client manifest
            printf("[upgrade] Removing entry for file: \"%s\" from client manifest\n", currentFileName);
            removeEntry(projname, currentFileName);
        }
        else if(currentUpdateLine[0] == 'A' || currentUpdateLine[0] == 'M'){
            //handle 'A' or 'M'-> fetch file from server manifest -> add to local proj directory -> add entry for file to client's manifest
            fetchData(sockfd, projname, currentFileName);
            fetchedFile = readInputFromServer(sockfd);
            if(fetchedFile == NULL){
                printf("[upgrade] Failed to fetch file \"%s\"!\n", currentFileName);
                return;
            }  
            if(fetchedFile[0][0] == 'f'){
                printf("[upgrade] Failed to fetch file \"%s\"!\n", currentFileName);
                return;
            }
            //fetched currentfile -> add to local proj
            createNewFile(currentFileName);
            overwriteOrCreateFile(currentFileName, fetchedFile[4]);
            //get fileversion from server manifest
            currentServerFileLine = getFileLineManifest(tempServerManifest, currentFileName, "-ps");
            currentServerFileVersionStr = nthToken(currentServerFileLine, 0, ' ');
            sscanf(currentServerFileVersionStr, "%d", &currentServerFileVersion);
            if(currentUpdateLine[0] == 'M'){ //delete old entry (M)
                printf("[upgrade] Removing old entry for file: \"%s\" from client manifest\n", currentFileName);
                removeEntry(projname, currentFileName);
            }
            //add entry of file w/ updated version to client's manifest
            add(projname, currentFileName, currentServerFileVersion);
        }
    }
    //update project version
    char* serverProjLine = getLineFile(tempServerManifest, 0);
    setLineFile(manifestPath, 0, serverProjLine);
    //Done processing->client manifest and proj directory should be up to date
    remove(tempServerManifest);
    printf("[upgrade] upgrade for proj \"%s\" was successful\n", projname);
}

void commit(char* projname, int sockfd){
    char* manifestPath = appendToStr(projname, "/.Manifest");
    char* updatePath = appendToStr(projname, "/.Update");
    char* conflictPath = appendToStr(projname, "/.Conflict");

    //Error checking:
    //1) non-empty update ->
    if(existsFile(updatePath) == 1 && isEmptyFile(updatePath) != 1){
        printf("[commit] Failed to commit...non-empty .Update exists\n");
    }
    if(existsFile(conflictPath) == 1){
        printf("[commit] Failed to commit... .Conflict file exists\n");
    }

    //fetch server's manifest:
    fetchData(sockfd, projname, manifestPath);
    char** output = readInputFromServer(sockfd);
    //receive manifest into char** output output[0] == status, output[1] == commandType, output[2] == projName, output[3] = fileName, output[4] = filedata; 
    if(output == NULL){
        printf("[commit] Failed!\n");
        free(manifestPath);
        free(updatePath);
        free(conflictPath);
        return;
    }
    if(output[0][0] == 'f'){
        printf("[commit] Failed!\n");
        free(manifestPath);
        free(updatePath);
        free(conflictPath);
        return;
    }
    char* serverFileData = output[4];
    printf("[commit] serverManifest is: \"%s\"\n", serverFileData);

    //Initiate temp server manifest
    char* tempServerManifest = appendToStr(projname, "/.serverManifest");
    createNewFile(tempServerManifest);
    overwriteOrCreateFile(tempServerManifest, serverFileData);

    //Create empty .Commit file
    char* commitPath = appendToStr(projname, "/.Commit");
    createNewFile(commitPath);
    open(commitPath, O_RDONLY|O_TRUNC, 00644);

    //Check manifest versions:
    char* serverManifestVersion = getLineFile(tempServerManifest, 0);
    char* clientManifestVersion = getLineFile(tempServerManifest, 0);
    if(compareString(serverManifestVersion, clientManifestVersion) != 0){ //different project versions -> need to update local project first
        printf("User must update project %s before committing\n");
        free(manifestPath);
        free(updatePath);
        free(conflictPath);
        remove(tempServerManifest);
        return;
    }

    //same manifest versions -> go through manifests
    int numLinesClientManifest = getNumLines(manifestPath);
    int numLinesServerManifest = getNumLines(tempServerManifest);
    char* currentClientLine;
    char* currentClientFileName;
    char* currentServerLine;
    char* currentServerFileName;
    //char* currentClientFileHash;
    char* storedClientHash;
    char* storedServerHash;
    char* liveClientHash;
    char* clientFileVersion;
    char* serverFileVersion;
    int clientFileVersionInt;
    int serverFileVersionInt;
    int failCommit = 0;
    char* lineToAppend;
    //first loop through client manifest handling cases:
    int i;
    for(i =1; i<numLinesClientManifest;i++){
        currentClientLine = getLineFile(manifestPath, i);
        currentClientFileName = nthToken(currentClientLine, 1, ' ');
        storedClientHash = nthToken(currentClientLine, 2, ' ');
        clientFileVersion = nthToken(currentClientLine, 0, ' ');
        sscanf(clientFileVersion, "%d", &clientFileVersionInt);
        if(( currentServerLine = getFileLineManifest(tempServerManifest, currentClientFileName, "-ps")) !=NULL){ // file in client's manifest exists in server's manifest
            storedServerHash = nthToken(currentServerLine, 2, ' ');
            liveClientHash = hashToStr(getHash(currentClientFileName));
            serverFileVersion = nthToken(currentServerLine, 0, ' ');
            sscanf(serverFileVersion, "%d", &serverFileVersionInt);
            if(compareString(storedClientHash, storedServerHash) == 0 && compareString(storedClientHash, liveClientHash) != 0){ //(M case)
                int replaceVersion = clientFileVersionInt + 1;
                lineToAppend = allocateCommitLine('M', currentClientFileName, liveClientHash, replaceVersion);
                if(lineToAppend!=NULL){
                    //append to commit
                    addToManifest(commitPath, lineToAppend);
                }
            }
            else if(compareString(storedClientHash, storedServerHash) != 0 && (serverFileVersionInt >= clientFileVersionInt)){ // Fail -> must sync repository
                failCommit = 1;
                break;
            }
        } 
        else{ // file in client's manifest does NOT exist in the server's manifest -> 
            int replaceVersion = clientFileVersionInt + 1;
            lineToAppend = allocateCommitLine('A', currentClientFileName, liveClientHash, replaceVersion);
            if(lineToAppend!=NULL){
                //append to commit
                addToManifest(commitPath, lineToAppend);
            }
        }
    }

    int j;
    for(j =1; j<numLinesServerManifest;j++){
        currentServerLine = getLineFile(tempServerManifest, j);
        currentServerFileName = nthToken(currentServerLine, 1, ' ');
        storedServerHash = nthToken(currentServerLine, 2, ' ');
        serverFileVersion = nthToken(currentServerLine, 0, ' ');
        sscanf(serverFileVersion, "%d", &serverFileVersionInt);
        if(( currentClientLine = getFileLineManifest(manifestPath, currentServerFileName, "-ps")) ==NULL){ // file in server's manifest does NOT exist in client's manifest
            int replaceVersion = serverFileVersionInt + 1;
            lineToAppend = allocateCommitLine('D', currentServerFileName, storedServerHash, replaceVersion);
            if(lineToAppend!=NULL){
                //append to commit
                addToManifest(commitPath, lineToAppend);
            }
        } 
    }

    if(failCommit == 1){
        printf("User must sync with the repository before committing changes\n");
        remove(commitPath);
        remove(tempServerManifest);
        free(commitPath);
        free(manifestPath);
        free(updatePath);
        free(conflictPath);
        free(tempServerManifest);
        return;
    }
    //Send commit
    sendCommand(sockfd, projname, "commit");
    sendData(sockfd, projname, commitPath);
    remove(tempServerManifest);
    free(commitPath);
    free(manifestPath);
    free(updatePath);
    free(conflictPath);
    free(tempServerManifest);
    printf("[commit] Commit was successful for \"%s\"\n", projname);
    return;
}

void push(char* projname, int sockfd){
    char* manifestPath = appendToStr(projname, "/.Manifest");
    char* updatePath = appendToStr(projname, "/.Update");
    char* commitPath = appendToStr(projname, "/.Commit");
    if(existsFile(commitPath) != 1){
       printf("[Push] failed to push project \"%s\"\n", projname);
       return;
    }
 
    //There is stuff to commit
    
    //Update client manifest and hashes
    int i;
    int numLinesCommit = getNumLines(commitPath);
    char* currentCommitLine;
    char* storedClientHash;
    char* currentCommitHash;
    char* currentCommitFileName;
    char* latestFileVersionStr;
    int latestFileVersionInt;
    int lineNumClientFile;
    char* lineNumClientFileStr;
    for(i=0;i<numLinesCommit;i++){
        currentCommitLine = getLineFile(commitPath, i);
        if(currentCommitLine== NULL){
            continue;
        }
        currentCommitFileName = nthToken(currentCommitLine, 1, ' ');
        currentCommitHash = nthToken(currentCommitLine, 2, ' ');
        latestFileVersionStr = nthToken(currentCommitLine, 3, ' ');
        latestFileVersionInt = strToNum(latestFileVersionStr);
        if((lineNumClientFileStr  = getFileLineManifest(manifestPath, currentCommitFileName, "-pi")) != NULL){
            lineNumClientFile = strToNum(lineNumClientFileStr);
            modifyManifest(projname, lineNumClientFile, "-v", latestFileVersionStr);
            modifyManifest(projname, lineNumClientFile, "-h", currentCommitHash);
        }
    }

    //send commit to server
    sendData(sockfd, projname, commitPath);
    char** output;
    // send necessary files from .Commit to server
    while((output = readInputFromServer(sockfd))!=NULL){
        sendData(sockfd, projname, output[3]); 
    }
    return;
}

void create(char* projname, int sockfd){
    //1) Client sendCommand -> "create" 
    sendCommand(sockfd, projname, "create");

    //START CLIENTSIDE: receiving message
    //Client: Receives sendServerToClientCreate string and then parses...-> 
    char** output = readInputFromServer(sockfd);
    if(output == NULL){
        printf("[create] Failed!\n");
        //close(sockfd);
        return;
    }
    if(output[0][0] == 'f'){
        printf("[create] Failed!\n");
        //close(sockfd);
        return;
    }

    char* serverFileData = output[4];
    printf("[create] serverManifest is: \"%s\"\n", serverFileData);
    int makeDir = mkdir(projname, 0777);
    char* manifestPath = appendToStr(projname, "/.Manifest");
    createNewFile(manifestPath);
    overwriteOrCreateFile(manifestPath, serverFileData);
    //close(sockfd);
    //if(totalNumBytesWritten == strlen(filedata)){
    printf("[create] Successfully created project: \"%s\"!\n", projname);
    //}
    // free(filedata);
    // free(manifestPath);
}

void destroy(char* projname, int sockfd){
    sendCommand(sockfd, projname, "destroy");
    char** output = readInputFromServer(sockfd);
    if(output == NULL){
        printf("[destroy] Failed!\n");
        //close(sockfd);
        return;
    }
    if(output[0][0] == 'f'){
        printf("[destroy] Failed!\n");
        //close(sockfd);
        return;
    } else if(output[0][0] == 's'){
        printf("[destroy] Successfully destroyed project: \"%s\"!\n", projname);
    }
}

void currentversion(char* projname, int sockfd){
    char* manifestPath = appendToStr(projname, "/.Manifest");
    //Client -> Server: sendCommand:  "<s><c><projNameLen>:<projname><commandLen>:<currentVersion>"
    //sendCommand(sockfd, projname, "currentversion");
    fetchData(sockfd, projname, manifestPath);
    char** output = readInputFromServer(sockfd);
    if(output == NULL){
        printf("[currentversion] Failed!\n");
        //close(sockfd);
        return;
    }
    if(output[0][0] == 'f'){
        printf("[currentversion] Failed!\n");
        //close(sockfd);
        return;
    }
    //Server -> Client: sendCommand: "<s><c><projNameLen>:<projname><dataLen>:<filenames:versions>" ... <filenames:versions> is the data internally separated by ':'
    //EX) "<s><c><5>:<proj0><46>:<Makefile:0:proj0/test0:1:proj0/sub0/subtest0:3>" gets sent from server to client
    //char* receivedStr = "Makefile:0:proj0/test0:1:proj0/sub0/subtest0:3:proj0/test1:35"; // replace hardcoded with what server sends client
    char* serverFileData = output[4];

    //make temp .currentVersionManifest 
    char* tempServerManifest = appendToStr(projname, "/.currentVersionManifest");
    createNewFile(tempServerManifest);
    overwriteOrCreateFile(tempServerManifest, serverFileData);

    int numLinesManifest = getNumLines(tempServerManifest);
    int i;
    for(i = 1;i<numLinesManifest;i++){
        //For each line output filedata and version
        char* currentLine = getLineFile(tempServerManifest, i);
        char* fileName = nthToken(currentLine, 1, ' ');
        char* versionStr = nthToken(currentLine, 0, ' ');
        printf("%s %s\n", fileName, versionStr);
    }
    remove(tempServerManifest);
    printf("[currentversion] Successfull!\n");
}

void history(char* projname, int sockfd){
    char* commitPath = appendToStr(projname, "/.Commit");
    sendCommand(sockfd, projname, "history");
    sendData(sockfd, projname, commitPath);
}

void rollback(char* projname, int version, int sockfd){
    
}
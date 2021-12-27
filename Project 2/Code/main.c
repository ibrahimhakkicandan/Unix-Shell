/* NAME      -      SURNAME    -    NUMBER
 * İBRAHİM HAKKI    CANDAN          150118061
 * ATİLA İLHAN      YATAĞAN         150118033
 * AYKUT            BAŞYİĞİT        150115854

*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <fcntl.h> /*for file io*/
#include <sys/stat.h> /*for file io*/

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
void aliasCheck(char*[]);
int backChilNumber = 0;
char * aliasConcat(char *[]);
void aliasDelete(int);
void comparePaths(char *,char *[]);
void comparePath(char *,char *,char*);
void printAliasList();
void setup(char [],char *[]);
void aliasAdd(char [],char *[]);
static int background;
pid_t childpid;
pid_t my_pid;
int checkIfExecuted;
int checkIfExecutet;
char *pathHolder[8];
char * aliasList[256];
char * commandList[256];


void sigStop(int p){
    signal(SIGTSTP,sigStop);
    //kill -SIGSTOP <childpid>
    //kill(childpid,SIGSTOP);
    fflush(stdout);
}
void sigInt(int p){
    signal(SIGINT,sigInt);
    printf("  Caught signal %d: SIGINT\n",p);
    fflush(stdout);
    exit(1);
}
int writeToFile1(char *path,char *args[],int lastIndex){

    int numberOfArgs = 0;   // holds the number of args in args.
    int i =0;
    while (args[i]!= NULL){
        numberOfArgs++;
        i++;
    }
    i = 0;

    if( (!strcmp(">",args[lastIndex-1])) && (!strcmp("<",args[lastIndex-3]) )){
        // sort < atiread > atiout
        i=0;
        char *newArgs[80];      // new args array to hold the args to be passed to execv.

        while( i < numberOfArgs-4){
            newArgs[i] = (char*) malloc(sizeof(char)* strlen(args[i]));
            strcpy(newArgs[i],args[i]);
            i++;
        }
        newArgs[i] = (char*) malloc(sizeof(char)* (strlen(args[i+1])));
        strcpy(newArgs[i],args[i+1]);
        newArgs[i+1] = '\0';      // null terminating the newArgs array.

#define CREATE_WRITEFLAGS (O_WRONLY | O_CREAT | O_TRUNC)             //flags to the fd.
#define CREATE_READFLAGS (O_WRONLY)
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

        int saved_stdin;                                       //holds the default fd table.
        saved_stdin= dup(0);
        int saved_stdout;
        saved_stdout= dup(1);

        int fdRead;
        int fdWrite;
        fdWrite = open(args[lastIndex], CREATE_WRITEFLAGS, CREATE_MODE);
        fdRead = open(args[lastIndex-2], CREATE_READFLAGS, CREATE_MODE);

        if (fdRead == -1) {

            fprintf(stderr,"Failed to open my.file");
            fflush(stderr);
            return 1;
        }

        if (dup2(fdRead, STDIN_FILENO) == -1) {

            fprintf(stderr,"Failed to redirect standard input");
            fflush(stderr);
            return 1;
        }

        if (close(fdRead) == -1) {
            fprintf(stderr,"Failed to close the file");
            fflush(stderr);
            return 1;
        }

        if (fdWrite == -1) {

            fprintf(stderr,"Failed to close the file");
            fflush(stderr);
            return 1;
        }

        if (dup2(fdWrite, STDOUT_FILENO) == -1) {

            fprintf(stderr,"Failed to redirect standard output");
            fflush(stderr);
            return 1;
        }

        if (close(fdWrite) == -1) {
            fprintf(stderr,"failed to close the file");
            fflush(stderr);
            return 1;
        }

        /* running the program */

        if(!(childpid = fork())){

            execv(path,newArgs);

        }
        else if(childpid==-1) { //failed to fork


            fprintf(stderr,"failed to fork a child\n");
            fflush(stderr);
        }
        else if(!background){

            waitpid(childpid,NULL,0);

        }
        else {
            //parent here
        }

        /* running the program */

        /* Restore stdout */
        dup2(saved_stdin, 0);
        close(saved_stdin);

        dup2(saved_stdout,1);
        close(saved_stdout);

#undef CREATE_WRITEFLAGS
#undef CREATE_READFLAGS //undef flags and mode to be able to declare them in the next iteration.
#undef CREATE_MODE

    }

    else if(!strcmp(">",args[lastIndex-1])){

        char *newArgs[80];      // new args array to hold the args to be passed to execv.
        while( i < numberOfArgs-2){
            newArgs[i] = (char*) malloc(sizeof(char)* strlen(args[i]));
            strcpy(newArgs[i],args[i]);
            //printf("%s \n",newArgs[i]);
            i++;
        }
        newArgs[i] = '\0';      // null terminating the newArgs array.

#define CREATE_FLAGS (O_WRONLY | O_CREAT | O_TRUNC)             //flags to the fd.
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

        int saved_stdout;                                       //holds the default fd table.
        saved_stdout = dup(1);

        int fd;
        fd = open(args[lastIndex], CREATE_FLAGS, CREATE_MODE);

        if (fd == -1) {

            fprintf(stderr,"failed to open the file");
            fflush(stderr);
            return 1;
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {

            fprintf(stderr,"Failed to redirect standard output");
            fflush(stderr);
            return 1;
        }

        if (close(fd) == -1) {
            fprintf(stderr,"failed to close the file");
            fflush(stderr);
            return 1;
        }
        /* running the program */


        if(!(childpid = fork())){

            execv(path,newArgs);

        }
        else if(childpid==-1) { //failed to fork


            fprintf(stderr,"failed to fork a child\n");
            fflush(stderr);
        }
        else if(!background){

            waitpid(childpid,NULL,0);

        }
        else {//parent here
        }

        /* running the program */

        /* Restore stdout */
        dup2(saved_stdout, 1);
        close(saved_stdout);

#undef CREATE_FLAGS //undef flags and mode to be able to declare them in the next iteration.
#undef CREATE_MODE

    }

    else if(!strcmp(">>",args[lastIndex-1])){

        char *newArgs[80];      // new args array to hold the args to be passed to execv.
        while( i < numberOfArgs-2){
            newArgs[i] = (char*) malloc(sizeof(char)* strlen(args[i]));
            strcpy(newArgs[i],args[i]);
            // printf("%s \n",newArgs[i]);
            i++;
        }
        newArgs[i] = '\0';      // null terminating the newArgs array.

#define CREATE_FLAGS (O_WRONLY | O_CREAT | O_APPEND)             //flags to the fd.
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

        int saved_stdout;                                       //holds the default fd table.
        saved_stdout = dup(1);

        int fd;
        fd = open(args[lastIndex], CREATE_FLAGS, CREATE_MODE);

        if (fd == -1) {
            fprintf(stderr,"Failed to open my.file");
            fflush(stderr);
            return 1;
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {

            fprintf(stderr,"Failed to redirect standard output");
            fflush(stderr);
            return 1;
        }

        if (close(fd) == -1) {
            fprintf(stderr,"failed to close the file");
            fflush(stderr);
            return 1;
        }
        /* running the program */

        if(!(childpid = fork())){

            execv(path,newArgs);

        }
        else if(childpid==-1) { //failed to fork

            fprintf(stderr,"failed to fork a child\n");
            fflush(stderr);
        }
        else if(!background){

            waitpid(childpid,NULL,0);

        }
        else {//parent here
        }

        /* running the program */

        /* Restore stdout */
        dup2(saved_stdout, 1);
        close(saved_stdout);

#undef CREATE_FLAGS //undef flags and mode to be able to declare them in the next iteration.
#undef CREATE_MODE

    }

    else if(!strcmp("<",args[lastIndex-1])){

        i=0;
        char *newArgs[80];      // new args array to hold the args to be passed to execv.

        while( i < numberOfArgs-2){
            newArgs[i] = (char*) malloc(sizeof(char)* strlen(args[i]));
            strcpy(newArgs[i],args[i]);
            i++;
        }
        newArgs[i] = (char*) malloc(sizeof(char)* (strlen(args[i+1])));
        strcpy(newArgs[i],args[i+1]);
        newArgs[i+1] = '\0';      // null terminating the newArgs array.


#define CREATE_FLAGS (O_WRONLY)             //flags to the fd.
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

        int saved_stdin;                                       //holds the default fd table.
        saved_stdin= dup(0);

        int fd;
        fd = open(args[lastIndex], CREATE_FLAGS, CREATE_MODE);

        if (fd == -1) {

            fprintf(stderr,"Failed to open my.file");
            fflush(stderr);
            return 1;
        }

        if (dup2(fd, STDIN_FILENO) == -1) {

            fprintf(stderr,"Failed to redirect standard input");
            fflush(stderr);
            return 1;
        }

        if (close(fd) == -1) {

            fprintf(stderr,"failed to close the file");
            fflush(stderr);
            return 1;
        }
        /* running the program */

        if(!(childpid = fork())){

            execv(path,newArgs);

        }
        else if(childpid==-1) { //failed to fork

            fprintf(stderr,"failed to fork a child\n");
            fflush(stderr);
        }
        else if(!background){

            waitpid(childpid,NULL,0);

        }
        else {
            //parent here
        }

        /* running the program */

        /* Restore stdout */
        dup2(saved_stdin, 0);
        close(saved_stdin);

#undef CREATE_FLAGS //undef flags and mode to be able to declare them in the next iteration.
#undef CREATE_MODE

    }



}
void execute(char inputBuffer[], char *args[]){ //to execute commands that entered


    char* pathGet= getenv("PATH"); // get $path folders
    int i = 0;
    char *path = strtok (pathGet, ":"); // split the paths by : character
    while (path != NULL)
    {
        pathHolder[i++] = path;
        path = strtok (NULL, ":");
    }
    checkIfExecuted = 0;
    checkIfExecutet = 0;
    char * sendAlias = "";
    for (i = 0; i < 8; ++i){ // traverse paths


        if(checkIfExecuted == 0){

            comparePaths(pathHolder[i],args);   // for searchs commands in the folders
        }
        if(checkIfExecutet == 0){

            for(int j = 0; j<=256;j++){ // for searching the entered commands in aliaslist or not
                if(aliasList[j] != NULL){
                    if(!strcmp(args[0],aliasList[j])){
                        sendAlias = (char*) malloc(sizeof(char)* strlen(commandList[j]));
                        char *tmpCommand = (char*) malloc(sizeof(char)*strlen(commandList[j]));
                        strcpy(tmpCommand,commandList[j]);
                        strcpy(sendAlias,strtok(tmpCommand, " ")); // get the command in the alias's real command
                        comparePath(pathHolder[i],sendAlias,commandList[j]); // for search the command in $path folders
                    }
                }

            }

        }


    }

}

void comparePath(char * path, char *cmmnd,char* alias)
{
    char tmp[128];
    *tmp = NULL;
    DIR * d = opendir(path); // open the path
    if(d==NULL) return; // if was not able, return
    struct dirent * dir; // for the directory entries
    while ((dir = readdir(d)) != NULL ) // if we were able to read something from the directory
    {
        if(dir-> d_type != DT_DIR) // if the type is not directory
        {


            //printf(" --%d--",background);
            if(!strcmp(dir->d_name,cmmnd)){
                checkIfExecutet++;

                strcpy(tmp,path);

                strcat(tmp,"/");

                strcat(tmp,alias);

                system(tmp); // execute the alias command
            }

        }
        else
        if(dir -> d_type == DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ) // if it is a directory
        {
            printf("%s\n", dir->d_name); // print its name
            char d_path[255];
            sprintf(d_path, "%s/%s", path, dir->d_name);
            comparePath(d_path,cmmnd,alias); // recall with the new path
        }
    }

    closedir(d); // finally close the directory
}
static int delete(int size, int index, char* array[]) // for deleting the command in aliaslist
{
    int x;
    for(x=index+1;x<size;x++) // for shifting the element because of deleted one index
    {
        strcpy(array[index], array[x]);
        index++;
    }

    size--;
    realloc(array, size);
    return size;
}
void comparePaths(char * path, char *args[]) // for executing the command --------PART A -------
{
    int lastIndex = 0;      // holds the last index of the args array.
    for(int j = 0;j <80;j++){
        if(args[j] == NULL){
            lastIndex = j-1;

            break;
        }
    }


    char tmp[128];
    *tmp = NULL;
    DIR * d = opendir(path); // open the path
    if(d==NULL) return; // if was not able, return
    struct dirent * dir; // for the directory entries
    while ((dir = readdir(d)) != NULL ) // if we were able to read something from the directory
    {
        if(dir-> d_type != DT_DIR) // if the type is not directory
        {

            //printf(" --%d--",background);
            if(!strcmp(dir->d_name,args[0])){
                checkIfExecuted++;
                strcpy(tmp,path);
                strcat(tmp,"/");
                strcat(tmp,args[0]);

                if((!strcmp(args[lastIndex-1],">")) || (!strcmp(args[lastIndex-1],"<")) || (!strcmp(args[lastIndex-1],">>")) ){ // --------FOR PART C ------
                    writeToFile1(tmp,args,lastIndex);
                }


                else if(!(childpid = fork())){

                    backChilNumber++;
                    execv(tmp,args);   // execute the commands


                }
                else if(childpid==-1) { //failed to fork


                    fprintf(stderr,"failed to fork a child\n");
                    fflush(stderr);

                }
                else if(!background){ // if the process in foreground

                    waitpid(childpid,NULL,0);

                }
                else {

                    //parent here
                }
            }

        }
        else
        if(dir -> d_type == DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ) // if it is a directory
        {
            printf("%s\n", dir->d_name); // print its name
            char d_path[255];
            sprintf(d_path, "%s/%s", path, dir->d_name);
            comparePaths(d_path,args); // recall with the new path
        }
    }

    closedir(d); // finally close the directory
}
void setup(char inputBuffer[], char *args[])
{

    int length, /* # of characters in the command line */
    i,      /* loop index for accessing inputBuffer array */
    start,  /* index where beginning of next command parameter is */
    ct;     /* index of where to place the next parameter into args[] */
    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);


    /* 0 is the system predefined file descriptor for stdin (standard input),
       which is the user's screen in this case. inputBuffer by itself is the
       same as &inputBuffer[0], i.e. the starting address of where to store
       the command that is read, and length holds the number of characters
       read in. inputBuffer is not a null terminated C-string. */

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */

/* the signal interrupted the read system call */
/* if the process is in the read() system call, read returns -1
  However, if this occurs, errno is set to EINTR. We can check this  value
  and disregard the -1 value */
    if ( (length < 0) && (errno != EINTR) ) {
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }

    for (i=0;i<length;i++){ /* examine every character in the inputBuffer */
        switch (inputBuffer[i]){
            case ' ':
            case '\t' :               /* argument separators */
                if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                    ct++;
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
                start = -1;
                break;
            case '\n':                 /* should be the final char examined */
                if (start != -1){
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
                break;
            default :             /* some other character */
                if (start == -1)
                    start = i;
                if (inputBuffer[i] == '&'){
                    background  = 1;
                    inputBuffer[i-1] = '\0';
                }

        } /* end of switch */
    }    /* end of for */

    args[ct] = NULL; /* just in case the input line was > 80 */
    if(backChilNumber > 0 && !strcmp(args[0],"exit"))
    {
        fprintf(stderr,"Im waiting for processes to be executed\n");
        return ;
    }
    else if(backChilNumber == 0 && !strcmp(args[0],"exit"))
    {
        perror("1");
        exit(1);
    }
    if((!strcmp("alias",args[0])) || (!strcmp("unalias",args[0]))){  //check if the first command alias/unalias or not
        if(!strcmp("alias",args[0])){
            if(args[1] != NULL && (!strcmp("-l",args[1]))){
                printAliasList();

            }
            else{
                aliasAdd(inputBuffer,args);

            }

        }
        else{
            for(int m = 0; m<257;m++){
                if(!strcmp(aliasList[m],args[1])){
                    aliasDelete(m);
                    break;
                }
            }

        }

    }
    else if((strcmp("alias",args[0])) && (strcmp("unalias",args[0]))){
        execute(inputBuffer, args);


    }
    else{
        fprintf(stderr,"Command not found");
        fflush(stderr);
    }

    /*for (i = 0; i <= ct; i++){
        printf("args %d = %s\n",i,args[i]);

    }*/

}

int main(void)
{
    char inputBuffer[MAX_LINE]; //*buffer to hold command entered *//*
    //* equals 1 if a command is followed by '&' *//*
    char *args[MAX_LINE/2 + 1]; //*command line arguments *//*

    /*struct sigaction childsignal;
    childsignal.sa_handler = controlBackground;
    childsignal.sa_flags = 0;
    if ((sigemptyset(&childsignal.sa_mask) == -1) || (sigaction(SIGCHLD, &childsignal, NULL) == -1))
    {
        fprintf(stderr,"Failed to set handler");
        return 1;
    }
*/
    /*
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = sigInt;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    */


    while (1){
        background = 0;

        //
        // fflush(stdin);

        printf("myshell: ");
        fflush(NULL);


        //*setup() calls exit() when Control-D is entered *//*
        setup(inputBuffer, args);
        //aliasAdd(inputBuffer,args,&head);
        printf("\n");

        /** the steps are:
        (1) fork a child process using fork()
        (2) the child process will invoke execv()
        (3) if background == 0, the parent will wait,
        otherwise it will invoke the setup() function again. */
    }

}




void aliasAdd(char inputBuffer[], char *args[]){ // for adding the alias to aliaslist

    char *command=  aliasConcat(args);

    int lastIndex = 0;
    for(int j = 0;j <80;j++){ // get the last index
        if(args[j] == NULL){
            lastIndex = j-1;

            break;
        }
    }


    for(int i = 0 ;i<256;i++){
        if(aliasList[i] == NULL){
            aliasList[i] = (char*)malloc(strlen(args[lastIndex])); //memory allocation for copying
            strcpy(aliasList[i] , args[lastIndex]); // copy the given alias from command line to aliaslist
            commandList[i] = (char*)malloc(strlen(command));
            strcpy(commandList[i] , command);
            break;
        }
    }
}
void aliasDelete(int index){ // remove an alias from alias list

    for(int x=index+1;x<257;x++) // for shifting indexes because of deleted one index
    {
        if(commandList[x] != NULL){
            strcpy(commandList[index], commandList[x]); // shifting
            strcpy(aliasList[index], aliasList[x]);
            index++;

        }
        else{
            aliasList[index] = '\0'; // add null character at the en of array
            commandList[index] = '\0';
            fflush(stdout);
            break;
        }

    }

}
void printAliasList(){ // for printing alias list
    for(int i = 0; i<257;i++){
        if(commandList[i] != NULL){
            printf("%s ",aliasList[i]);
            printf("\"%s\" \n",commandList[i]);
            fflush(stdout);
        }
        else{break;};
    }
}
char* strdup(const char* org) // for returning some char array
{
    if(org == NULL) return NULL;

    char* newstr = (char*)malloc(strlen(org)+1);
    char* p;

    if(newstr == NULL) return NULL;

    p = newstr;

    while(*org) *p++ = *org++; /* copy the string. */
    return newstr;
}
char * aliasConcat(char *args[]){ // to get rid of quotes given in the command line
    char argsContent[80];

    memset(&argsContent[0], 0, sizeof(argsContent));

    char returnValue[80];
    memset(&returnValue[0], 0, sizeof(returnValue));


    for(int i = 1;i<80;i++){    //iterate args

        if(args[i+1] != NULL){ // check args index null or not

            strcpy(argsContent,args[i]); // for shifting the indexes
            strcat(returnValue,argsContent);
            strcat(returnValue," ");
        }
        else{
            memset(&argsContent[0], 0, sizeof(argsContent));
            break;
        }
    }
    int i,len=strlen(returnValue);
    for(i=1;i<len-1;i++)
    {
        returnValue[i-1]=returnValue[i];
    }
    returnValue[i-2]='\0'; // delete the second quote and assign it to null character
    char string[80] ;
    strcpy(string,returnValue);

    return strdup(returnValue);
}





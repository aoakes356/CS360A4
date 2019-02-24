#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "connect.h"

int main(int argc, char** argv){
    // Use buffer to store executables and their corresponding arguments.
    // Too big to go on stack, use malloc.
    char*** buffer = malloc(sizeof(char**)*MAX_CHAIN_SIZE);
    int fds[2];
    for(int i = 0 ; i < MAX_CHAIN_SIZE; i++){
        buffer[i] = calloc(MAX_ARGS,sizeof(char*));
        for(int j = 0; j < MAX_ARGS; j++){
            buffer[i][j] = calloc(MAX_STR,sizeof(char));
        }
    }

    if(pipe(fds)){
        fprintf(stderr,"Failed to create a new pipe!\n");
        return 1;
    }
    int res = getArgs(argv,buffer,argc);
    if(res < 0){
        return 1;
    }

    //for(int k = 1; k < res; k++){
        //buffer[k] will be an argument list.
        
    //}
    if(!fork()){
        printf("Child!\n");
        if(!fork()){
            printf("Child 2!\n");
            close(fds[0]);
            fprintf(stdout,"EXE1: %s\n",buffer[0][0]);
            dup2(fds[1],1);
            execvp(buffer[0][0],buffer[0]);
        }else{
            while(wait(NULL) > 0);
            printf("Child!\n");
            close(fds[1]);
            dup2(fds[0],0);
            printf("EXE2: %s\n",buffer[1][0]);
            execvp(buffer[1][0],buffer[1]);
        }
    }else{
        while(wait(NULL) > 0)printf("IN A CAN");
        printf("Parent!\n");
    }    



    // Free the buffer.
    for(int i = 0 ; i < MAX_CHAIN_SIZE; i++){
        for(int j = 0; j < MAX_ARGS; j++){
            if(buffer[i][j] != NULL && buffer[i][j][0] != '\0'){
                printf("%s ",buffer[i][j]);
            }
            free(buffer[i][j]);
        }
        if(i <= res){
            printf("\n");
        }
        free(buffer[i]); 
    }
    free(buffer);


}

int getArgs(char** arguments, char*** buffer, int argc){   // Returns number of executables or -1 on error
    int exeCount = 0;
    int argCount = 0;
    if(argc <= 1) {
        fprintf(stderr,"Invalid argument count\n");
        printf("Invalid argument count\n");
        return -1;
    } 
    for(int i = 1; i < argc; i++){
        //arguments[i]
        if(!strcmp(arguments[i],":")){
            buffer[exeCount++][argCount] = NULL;
            argCount = 0;
        }else{
            strcpy(buffer[exeCount][argCount++], arguments[i]);
        }
        if(exeCount > MAX_CHAIN_SIZE){
            fprintf(stderr,"Too many executables! Buffer size exceeded.\n");
            printf("Too many executables! Buffer size exceeded.\n");
            return -1;
        }
    }
    return exeCount;
}

int runExe(char** args){ // Runs the executable stored in args[0], last element in args should be NULL.
    return 0;
}

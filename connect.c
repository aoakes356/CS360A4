#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "connect.h"

int main(int argc, char** argv){
    // Use buffer to store executables and their corresponding arguments.
    // Too big to go on stack, use malloc.
    // Would have been necessary had I decided to try to make it work for numerous executables.
    char*** buffer = malloc(sizeof(char**)*MAX_CHAIN_SIZE);
    int fds[2];
    int stat;
    for(int i = 0 ; i < MAX_CHAIN_SIZE; i++){
        buffer[i] = calloc(MAX_ARGS,sizeof(char*));
        for(int j = 0; j < MAX_ARGS; j++){
            buffer[i][j] = calloc(MAX_STR,sizeof(char));
        }
    }
    // Parse the arguments into executables and their corresponding arguments.
    int res = getArgs(argv,buffer,argc);
    // res contains the number of executables.
    if(res == 2){
        if(buffer[1][0] == NULL){
            if(!fork()){
                execvp(buffer[0][0],buffer[0]);
                fprintf(stderr,"failed to execute %s! %s\n",buffer[0][0],strerror(errno));
                return 1;
            }else{
                wait(NULL);
            }    
        }else{
            if(pipe(fds)){
                fprintf(stderr,"Failed to create a new pipe!\n");
                return 1;
            }
            if(!fork()){
                dup2(fds[1],1);
                if(close(fds[0]) < 0) fprintf(stderr,"%s\n",strerror(errno));
                execvp(buffer[0][0],buffer[0]);
                fprintf(stderr,"failed to execute %s! %s\n",buffer[0][0],strerror(errno));
                return 1;
            }else{
                wait(NULL);
                dup2(fds[0],0);
                if(close(fds[1]) < 0) fprintf(stderr,"%s\n",strerror(errno));
                execvp(buffer[1][0],buffer[1]);
                fprintf(stderr,"failed to execute %s! %s\n",buffer[1][0],strerror(errno));
                return 1;
            }
        }
    }
    // if only one executable just run it.
    if(res == 1){
        if(!fork()){
            execvp(buffer[0][0],buffer[0]);
            fprintf(stderr,"failed to execute %s! %s\n",buffer[0][0],strerror(errno));
            return 1;
        }else{
            wait(NULL);
        }    
    }

    // Free the buffer.
    for(int i = 0 ; i < MAX_CHAIN_SIZE; i++){
        for(int j = 0; j < MAX_ARGS; j++){
            if(buffer[i][j] != NULL && buffer[i][j][0] != '\0'){
                //printf("%s ",buffer[i][j]);
            }
            free(buffer[i][j]);
        }
        if(i < res){
            //printf("\n");
        }
        free(buffer[i]); 
    }
    free(buffer);
    return 0;

}

// Originally planned to go for extra credit but I ran out of time, I will leave thise here though.
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
    buffer[exeCount++][argCount] = NULL;
    return exeCount;
}

int runExe(char** args){ // Runs the executable stored in args[0], last element in args should be NULL.
    return 0;
}

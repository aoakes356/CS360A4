#ifndef CONNECT_H
#define CONNECT_H

#define MAX_CHAIN_SIZE 128
#define MAX_ARGS 256
#define MAX_STR 1024
    
// Stores each executable and its arguments in buffer

int getArgs(char** arguments, char*** buffer, int argc);   // Returns number of executables or -1 on error

int runExe(char** args); // Runs the executable stored in args[0], last element in args should be NULL.









#endif

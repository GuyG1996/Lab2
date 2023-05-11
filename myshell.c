#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "LineParser.h"
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <errno.h>

//Ron Hadad 209260645, Guy Ginat 206922544

bool debugMode = false;

void execute(cmdLine *pCmdLine){
    if(strcmp(pCmdLine->arguments[0], "quit") == 0){//case "quit"
        freeCmdLines(pCmdLine);
        exit(0);
    }
    if(strcmp(pCmdLine->arguments[0], "cd") == 0){//case "cd"
        if (chdir(pCmdLine->arguments[1]) != 0) {//case error chainging directory
            fprintf(stderr, "failed changing directory \n");
        }
    }
    //special signals for a child procces:
    if(strcmp(pCmdLine->arguments[0], "suspend") == 0){
        int pid = atoi(pCmdLine->arguments[1]);
        if (kill(pid, SIGTSTP) == -1) {
            fprintf(stderr, "failed to suspend process %s \n", pCmdLine->arguments[1]);
        }
    }
    else if(strcmp(pCmdLine->arguments[0], "wake") == 0){
        int pid = atoi(pCmdLine->arguments[1]);
        if (kill(pid, SIGCONT) == -1) {
            fprintf(stderr, "failed to wake process %s \n", pCmdLine->arguments[1]);
        }
    }
    else if(strcmp(pCmdLine->arguments[0], "kill") == 0){
        int pid = atoi(pCmdLine->arguments[1]);
        if (kill(pid, SIGTERM) == -1) {
            fprintf(stderr, "failed to kill process %s \n", pCmdLine->arguments[1]);
        }
    }
    else{// if not special signal:
        int pid;
        pid = fork();
        if (pid == 0) {
            // Child process
            if(pCmdLine->inputRedirect != NULL){
                int new_stdin = open(pCmdLine->inputRedirect, O_RDONLY);
                if (new_stdin < 0) {
                    fprintf(stderr, "failed to open inputRedirect file  \n");
                    freeCmdLines(pCmdLine);
                    exit(0);
                }
                dup2(new_stdin, STDIN_FILENO);
            }
            if(pCmdLine->outputRedirect != NULL){
                int new_stdout = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (new_stdout < 0) {
                    fprintf(stderr, "failed to open outputRedirect file \n");
                    freeCmdLines(pCmdLine);
                    exit(0);
                }
                dup2(new_stdout, STDOUT_FILENO);
            }
            
            if(execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1){
                perror("could not execute the command \n");
                freeCmdLines(pCmdLine);
                exit(EXIT_FAILURE);
            }
        } else if (pid > 0) {
            // Parent process
            if(debugMode){
                fprintf(stderr, "PID : %d \n", pid);
                fprintf(stderr, "Child process executing command : %s \n", pCmdLine->arguments[0]);
            }
            if(pCmdLine->blocking == '1'){
                waitpid(pid, NULL, 0);
            }
        } else {
            // Fork failed
            fprintf(stderr, "Fork failed!\n");
            freeCmdLines(pCmdLine);
            exit(0);
        }
    }
}

int main(int argc, char *argv[]){
    if(argc>1 && strcmp(argv[1],"-d") == 0){
        debugMode = true;
    }
    while(1){
        char currWrkDrctr[PATH_MAX];
        if (getcwd(currWrkDrctr, sizeof(currWrkDrctr)) != NULL) {
            printf("%s$ ", currWrkDrctr);
            char inputLine[2048];
            if(fgets(inputLine, sizeof(inputLine), stdin) != NULL){
                if (strcmp(inputLine,"\n")==0){
                    fprintf(stderr, "could not execute the command \n");
                    continue;
                }
                cmdLine* currcmdLine = parseCmdLines(inputLine);
                if(currcmdLine == NULL){
                    fprintf(stderr, "couldn't parse command\n");
                    continue;
                }
                execute(currcmdLine);
            }
            else{
                printf("failed to read stdin");
            }
        }
        else{
            printf("failed to find current working directory");
        }
    }
}
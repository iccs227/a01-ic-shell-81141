#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "executor.h"
#include <fcntl.h>
#include "jobs.h"

extern int fg_pid;
extern int exit2;
#define MAX_CMD_BUFFER 255
void milestone3(char *buffer, pid_t shell_pid, bool background) {
    int pid;

    char full_cmd[MAX_CMD_BUFFER]; // this is temporary fix if i have time i will change 
    strncpy(full_cmd, buffer, sizeof(full_cmd));
    full_cmd[sizeof(full_cmd)-1] = '\0';

    char *prog_argv[32]; // safe upper bound 31 + NULL
    static int job_number = 1;

    char *token = strtok(buffer, " "); // im splitting the command 
    int i = 0; // this is for my while loop 
    while (i < 31 && token) {
        prog_argv[i++] = token; // save the current in the list 
        token = strtok(NULL, " "); // move to the next word
    }
    prog_argv[i] = NULL; // make the ending null for execvp()

    if ((pid = fork()) < 0) {
        perror("Fork failed");
        exit(errno);
    }
    if (!pid) {  // child
        // fixing the scheduling race issue and only giving the terminal to the fg 
        // so that the bg can't steal it anymore    
        if(setpgid(0, 0) < 0){
            perror("setgpid");
        }
        if(!background){
            if(tcsetpgrp(0 , getpid())){
                perror("tcsetpgrp");
            }
        }

        // i used ajarn's template code form the reading part
        for (int i = 0; prog_argv[i]; i++) {
            if (strcmp(prog_argv[i], "<") == 0 && prog_argv[i+1]) {// check if there is < and if there is a file .txt in the next argv
                int in = open(prog_argv[i+1], O_RDONLY);
                if (in < 0) { perror(prog_argv[i+1]); exit(errno); }
                dup2(in, 0);
                close(in);
                prog_argv[i] = NULL; //Remove < and filename from argv so execvp only see real args
                break;
            }
        }
        for (int i = 0; prog_argv[i]; i++) {
            if (strcmp(prog_argv[i], ">") == 0 && prog_argv[i+1]) {
                int out = open(prog_argv[i+1],O_CREAT|O_TRUNC|O_WRONLY, 0666);
                if (out < 0) { perror(prog_argv[i+1]); exit(errno); }
                dup2(out, 1);
                close(out);
                prog_argv[i] = NULL;  
                break;
            }
        }
        
        execvp(prog_argv[0], prog_argv);
        fprintf(stderr, "Bad comment"); // i had to change this since it exits 
        fflush(stderr);
        tcsetpgrp(0, getppid()); // return to the main terminal not exit 
        exit(127); // command-not-found 
    }
    else { // parent
        //* prevents any race condition (call child again incase)
        setpgid(pid, pid);

        if(background){
            int job_num = job_number++;
            int jid = adding_jobs(pid, full_cmd);
            setpgid(pid, pid); 
            printf("[%d] %d\n", jid, pid);
            fflush(stdout);
            tcsetpgrp(0, shell_pid);  
            return;
        }
        else{
            tcsetpgrp(0, pid);
            fg_pid = pid;

            int status; 
            waitpid(pid, &status, WUNTRACED);
            tcsetpgrp(0, shell_pid); // return control to the shell 
            fg_pid = 0;

            if (WIFSTOPPED(status)) {
                printf("\nSuspended\n");
            }
            if (WIFEXITED(status)) {
                exit2 = WEXITSTATUS(status);
            }
        }
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "executor.h"

extern int fg_pid;
extern int exit2;

void milestone3(char *buffer, pid_t shell_pid) {
    int pid;
    char *prog_argv[32]; // safe upper bound 31 + NULL

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
        setpgid(0, 0); // Create a new process group for the child
        tcsetpgrp(0, getpid()); // give terminal control to the new child 

        execvp(prog_argv[0], prog_argv);
        fprintf(stderr, "Bad comment"); // i had to change this since it exits 
        fflush(stderr);
        tcsetpgrp(0, getppid()); // return to the main terminal not exit 
        exit(127); // command-not-found 
    }
    else { // parent
        //* prevents any race condition (call child again incase)
        setpgid(pid, pid);
        tcsetpgrp(0, pid);
        fg_pid = pid;

        int stop;
        waitpid(pid, &stop, WUNTRACED);
        tcsetpgrp(0, shell_pid); // return control to the shell 
        fg_pid = 0;

        if (WIFSTOPPED(stop)) {
            printf("\nSuspended\n");
        }
        if (WIFEXITED(stop)) {
            exit2 = WEXITSTATUS(stop);
        }
    }
}

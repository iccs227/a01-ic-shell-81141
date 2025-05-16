/* ICCS227: Project 1: icsh
 * Name: Hanna Hahn
 * StudentID: 6481334
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h> 
#include <stdbool.h>
#include <signal.h>
#include <termios.h>


#define MAX_CMD_BUFFER 255

int fg_pid;
int exit2;

void signal_Z (int sig_num){
    if(fg_pid > 0) {
        kill(-fg_pid,SIGTSTP);
    }
    // signal(SIGTSTP,signal_Z);
    // printf("You pressed Ctrl+Z\n");
}
void signal_C (int sig_num){
    if(fg_pid > 0) {
        kill(-fg_pid,SIGINT);
    }
    // signal(SIGINT,signal_C);
    // printf("You pressed Ctrl+C\n");
}

int main(int argc, char *argv[]) {
    FILE *file = stdin; // reading file and also initialize it 
    bool active = true; 
    int exits = 0; // return the number from the exit 

    char buffer[MAX_CMD_BUFFER];
    char last_cmd[MAX_CMD_BUFFER] = {0};

    if (argc == 2) {  // checking if there's more than 1 argument rn 
        file = fopen(argv[1], "r");  // read the file 
        if (!file) {
            perror(argv[1]);
            printf("No file detected");
            return 1; 
        }
        active = false;
    } else if (argc > 2) {
        // too many arguments: show usage and exit
        fprintf(stderr, "Usage: %s [scriptfile]\n", argv[0]);
        return 1;
    }
    

    /*
    this is for the shell
    preparing shell to control the terminal and
    give it to the child later after that we reclaim it .
    *-making shell the current controler  and fg
    */ 
   pid_t shell_pid = getpid();
    setpgid(shell_pid,shell_pid); 
    tcsetpgrp(0,shell_pid); //make shell the fg group for terminal 

    //!Debugging issue 
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    signal(SIGTSTP,signal_Z);
    signal(SIGINT,signal_C);
    while (1) {
        if(active){
            printf(":-) \n");
            printf("icsh $ ");
            fflush(stdout); //force output to be written to the file
        }

        if (!fgets(buffer, MAX_CMD_BUFFER, file))
            break;

        buffer[strcspn(buffer, "\n")] = '\0'; // removing the \n so that it's easier to compare

        // base case
        if (buffer[0] == '\0')
            continue;

        if (strcmp(buffer, "!!") == 0) {
            if (last_cmd[0] != '\0') {
                printf("%s\n", last_cmd);
                strcpy(buffer, last_cmd);
            } else{
                continue;
            } 
        }

        if (strncmp(buffer, "echo ", 5) == 0) { // 5 because at the end there's a space
            printf("%s\n", buffer + 5); // move past the echo and only take what comes after that
        }
        else if (strncmp(buffer, "exit ", 5) == 0) {
            exits = atoi(buffer + 5); // atoi converts the string into an int "300" -> 300
            printf(">:-(\n");
            fflush(stdout); 
            break; // exit loop; will return exits % 256 below
        }
        else if (strcmp(buffer, "echo $?") == 0) { // return last exit statement
            printf("%d\n", exit2);
        }
        // else {
        //     printf("bad command\n");
        // }
        else{
            int pid;
            char *prog_argv[32]; // safe upper bound 31 + NULL

            char *token = strtok(buffer," "); // im splitting the command 
            
            int i = 0; // this is for my while loop 
            while (i < 31 && token){
                prog_argv[i++] = token; // save the current in the list 
                token = strtok(NULL," ");// move to the next word
            }
            prog_argv[i] = NULL; // make the ending null for execvp()

            if ((pid=fork()) < 0)
            {
                perror ("Fork failed");
                exit(errno);
            }
            if (!pid)  //child
            {
                setpgid(0, 0); // Create a new process group for the child
                tcsetpgrp(0,getpid()); // give terminal control to the new child 

                execvp (prog_argv[0], prog_argv);
                fprintf(stderr,"Bad comment"); // i had to change this since it exits 
                fflush(stderr);
                tcsetpgrp(0, getppid()); // return to the main terminal not exit 
                exit(127); //command-not-found 
            }

            else 
            {
                //parent
                //* prevents any race condition (call child again incase)
                setpgid(pid,pid);
                tcsetpgrp(0,pid);
                fg_pid = pid;

                int stop ;
                waitpid (pid, &stop, WUNTRACED); 
                tcsetpgrp(0,shell_pid); // return control to the shell 
                fg_pid = 0;

                if (WIFSTOPPED(stop)){
                    printf("\nWorking\n");
                }               
            }
                }

        if (strcmp(buffer, "!!") != 0) // if it's not !! at the end we then update last
            strcpy(last_cmd, buffer);
    }

    if (file != stdin) { // in case if we opened the file we have to close it 
        fclose(file);
    }

    return exits * 0xFF; // keep in the range of 0–255
}


// run using                        ./icsh.exe tesh.sh
// to check the exit number run     echo $?
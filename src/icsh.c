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
#include "signals.h"
#include "executor.h"
#include "jobs.h"


#define MAX_CMD_BUFFER 255

int fg_pid;
int exit2;

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
        fprintf(stderr, "Usage: %s [scriptfile]\n", argv[0]);
        return 1;
    }
    
    pid_t shell_pid = getpid();
    setup_shell_signals(shell_pid);  // make shell fg and register handlers
    initialize();  // this is from the jobs 
    while (1) {
        if(active){
            printf("₊˚⊹  ♡︎ \n"); 
            printf(" `· . ꔫ \n"); 
            printf("icsh $ ");
            fflush(stdout); //force output to be written to the file
        }

        if (!fgets(buffer, MAX_CMD_BUFFER, file))
            break;

        buffer[strcspn(buffer, "\n")] = '\0'; // removing the \n so that it's easier to compare

        // base case
        if (buffer[0] == '\0')
            continue;

        bool history = false; // see if the command came from the history or not 

        if (strncmp(buffer, "!!", 2) == 0) {
            char array[MAX_CMD_BUFFER];
            char *space = buffer + 2; // detect what is after !!
            while (isspace(*space)){
                space++; // removing the white spaces after !!
            }
            if(*space){ // this is for when there's something behind the !!
                snprintf(array, sizeof(array), "%s %s", last_cmd, space);
            }
            else{ // for when it's only !! 
                snprintf(array, sizeof(array), "%s", last_cmd);
            }
            printf("%s\n", array);
            strcpy(buffer, array);
            history = true; // see if the command came from the history or not 
        }
        if (!history) {
            strcpy(last_cmd, buffer); // if it's not from the history we will record it 
        }
        if (strncmp(buffer, "echo ", 5) == 0) {
            // here is where i fix the echo > output.txt so that i doesn't treat all of it as a string 
            // and redirect it properly in milestone 3
            if (strchr(buffer, '>') != NULL || strchr(buffer, '<') != NULL) { 
                milestone3(buffer, shell_pid, false);
            }else{
                printf("%s\n", buffer + 5);
            }
            continue;
        }
        else if (strncmp(buffer, "exit ", 5) == 0) {
            exits = atoi(buffer + 5); // atoi converts the string into an int "300" -> 300
            printf("   /ᐢ⑅ᐢ\   ♡   ₊˚  \n");
            printf("꒰ ˶• ༝ •˶꒱       ♡‧₊˚    ♡\n");
            printf("./づ~ :¨·.·¨:     ₊˚  \n");
            printf("       `·..·‘    ₊˚   ♡\n");
            fflush(stdout); 
            break; // exit loop; will return exits % 256 below
        }
        else if (strcmp(buffer, "echo $?") == 0) { // return last exit statement
            printf("%d\n", exit2);
        }
        else if (strcmp(buffer, "jobs") == 0){
            printing_checking_status();
            continue;
        }
        else if(strncmp(buffer, "bg %" , 4)==0){
            int job_id = atoi(buffer + 4);
            jobs_t *j = find_job_id(job_id); // i'm losing it mentally 
            if(j){
                kill(-j->pgid, SIGCONT); //wake it up to sontinue after being stopped
                j->status = Running;
                printf("[%d] %s &\n", j->id, j->cmd);
            }else{
                printf("Error, no jobs");
            }
            continue;
            }
        else if(strncmp(buffer, "fg %" , 4)==0){
            int job_id = atoi(buffer + 4);
            jobs_t *j = find_job_id(job_id); // still losing it mentally 
            if(j){

                kill(-j->pgid, SIGCONT);
                tcsetpgrp(0, j->pgid); // give the job the terminal so she can pop off
                // similar to the parent in executor.c
                fg_pid = j->pgid;
                int status;
                waitpid(-j->pgid,&status,WUNTRACED); // wait for the process to be done
                printf("%s\n", j->cmd);
                fflush(stdout);
                
                // here i reclaim the terminal after the process is done 
                tcsetpgrp(0,shell_pid);
                fg_pid = 0;

                //now imma make the crtl z work here so can cancel it out 
                 if (WIFSTOPPED(status)) {
                    j->status = Stopped;
                    printf("[%d]+  Stopped\t%s\n", j->id, j->cmd);
                } else {
                    j->status = Done;
                    remove_job(job_id); 
                }
            }
            continue;
            }
            else if (strcmp(buffer, "hangman") == 0) {
                hangman_game();
                continue;
            } 
        else {
            // ex:  "sleep 5 & "
            //* purpose of this is to give a clean code so that the process can run easier
            bool background = false; // here is where i start m6 the & part
            // first i will remove the white trail so i can get the last posi. easier
            int len = strlen(buffer);
            while (len > 0 && isspace((unsigned char)buffer[len-1])){ //"sleep 5 &"
                buffer[--len] = '\0'; // reduce the size to take out the whitetrailing  
            }
            if (len > 0 && buffer[len-1] == '&') {
                background = true;
                buffer[--len] = '\0'; // "sleep 5 "
                buffer[--len] = '\0'; // "sleep 5" 
                // if user puts more then 1 space then won't work skill issue
                // easy fix could be to put the while loop again but im lazy. 
            }

            milestone3(buffer, shell_pid, background);  // Milestone 3 
        }
    } 

    if (file != stdin) { // in case if we opened the file we have to close it 
        fclose(file);
    }

    return exits; 
}

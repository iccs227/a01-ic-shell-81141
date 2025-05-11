/* ICCS227: Project 1: icsh
 * Name: Hanna Hahn
 * StudentID: 6481334
*/

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>   
#include <ctype.h> 
#include <stdbool.h>

#define MAX_CMD_BUFFER 255

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

    // printf("Hello\n");
    // fflush(stdout);
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
        else {
            printf("bad command\n");
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
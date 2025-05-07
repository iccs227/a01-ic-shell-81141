/* ICCS227: Project 1: icsh
 * Name: Hanna Hahn
 * StudentID: 6481334
 */

 #include <stdio.h>
 #include <stdlib.h>   
 #include <string.h>   
 #include <ctype.h> 

#define MAX_CMD_BUFFER 255

int main() {
    char buffer[MAX_CMD_BUFFER];
    char last_cmd[MAX_CMD_BUFFER] = {0};

    // printf("Hello\n");
    // fflush(stdout);
    while (1) {
        printf(":-) \n");
        printf("icsh $ ");
        fflush(stdout); //force output to be written to the file

        if (!fgets(buffer, MAX_CMD_BUFFER, stdin))
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
            printf("%s\n", buffer + 5); // move pass the echo and only take what comes after that
        }
        else if (strncmp(buffer, "exit ", 5) == 0) {
            int code = atoi(buffer + 5); // atoi converts the string into an int "300" --> 300. would not work its a string like "abc"
            printf(">:-(\n");
            fflush(stdout); 
            return code % 256; // keeps it in the range of 255 
            // 256 cuz we count 0 too in 0-255
        }
        else {
            printf("bad command\n");
        }

        if (strcmp(buffer, "!!") != 0) //if it's not !! at the end we then update last
            strcpy(last_cmd, buffer);
    }
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    char *command = NULL;
    size_t size = 0;
    ssize_t length;

    while (1) {
        printf("Enter command: ");
        fflush(stdout);

        length = getline(&command, &size, stdin);

        if (length == -1) {
            printf("\nExiting program...bye!\n");
            break;
        }

        // Remove newline character
        if (length > 0 && command[length - 1] == '\n') {
            command[length - 1] = '\0';
        }

        // Check if user wants to quit
        if (strcmp(command, "quit") == 0) {
            printf("Exiting program...bye!\n");
            break;
        }

        // Ignore empty commands
        if (command[0] == '\0') {
            continue;
        }

        // Execute command using popen
        FILE *pipe = popen(command, "r");

        if (pipe == NULL) {
            perror("popen");
            continue;
        }

        char output[1024];

        // Read and display command output
        while (fgets(output, sizeof(output), pipe) != NULL) {
            fputs(output, stdout);
        }

        // Close pipe and wait for command
        int status = pclose(pipe);

        if (status == -1) {
            perror("pclose");
        }
    }

    free(command);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_ARGS 128
#define MAX_LINE 1024

int main(int argc, char *argv[])
{
    FILE *input;
    FILE *log;
    char line[MAX_LINE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_filename>\n", argv[0]);
        return 1;
    }

    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("fopen input");
        return 1;
    }

    log = fopen("output.log", "w");
    if (log == NULL) {
        perror("fopen output.log");
        fclose(input);
        return 1;
    }

    while (fgets(line, sizeof(line), input) != NULL) {
        char original[MAX_LINE];
        char *args[MAX_ARGS];
        char *token;
        int count = 0;
        int status;
        pid_t pid;
        time_t start_time, end_time;
        char start_string[32];
        char end_string[32];
        char *newline;

        /* Remove the newline at the end of the command. */
        newline = strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        /* Skip empty lines. */
        token = line;
        while (*token == ' ' || *token == '\t') {
            token++;
        }

        if (*token == '\0') {
            continue;
        }

        /* Preserve the original command for the log. */
        snprintf(original, sizeof(original), "%s", token);

        /* Split the command into arguments. */
        token = strtok(token, " \t");

        while (token != NULL && count < MAX_ARGS - 1) {
            args[count++] = token;
            token = strtok(NULL, " \t");
        }
        args[count] = NULL;

        if (count == 0) {
            continue;
        }

        /* Record the start time before creating the child. */
        start_time = time(NULL);
        snprintf(start_string, sizeof(start_string), "%s",
                 ctime(&start_time));
        start_string[strcspn(start_string, "\n")] = '\0';

        pid = fork();

        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            /* Child process: execute the requested command. */
            execvp(args[0], args);

            /* Reached only if execvp fails. */
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        /* Parent process: wait for the child to finish. */
        while (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            break;
        }

        /* Record the end time after the child finishes. */
        end_time = time(NULL);
        snprintf(end_string, sizeof(end_string), "%s",
                 ctime(&end_time));
        end_string[strcspn(end_string, "\n")] = '\0';

        /* Write the command and timestamps, separated by tabs. */
        fprintf(log, "%s\t%s\t%s\n",
                original, start_string, end_string);
        fflush(log);

        if (WIFEXITED(status)) {
            printf("Command completed with status %d: %s\n",
                   WEXITSTATUS(status), original);
        } else if (WIFSIGNALED(status)) {
            printf("Command terminated by signal: %s\n", original);
        }
    }

    fclose(input);
    fclose(log);

    return 0;
}

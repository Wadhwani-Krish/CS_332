#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFSIZE 4096

int main(int argc, char *argv[]) {
    int file1, file2;
    long int n;
    char buf[BUFFSIZE];

    if (argc != 3) {
        printf("Usage: %s <file1> <file2>\n", argv[0]);
        exit(-1);
    }

    /* Check if the two filenames are the same */
    if (strcmp(argv[1], argv[2]) == 0) {
        printf("Error: The two filenames must be different\n");
        exit(-1);
    }

    /* Open the second file for reading */
    file2 = open(argv[2], O_RDONLY);

    if (file2 == -1) {
        printf("Error with source file open\n");
        exit(-1);
    }

    /* Open the first file for appending */
    file1 = open(argv[1], O_WRONLY | O_APPEND);

    if (file1 == -1) {
        printf("Error with destination file open\n");
        close(file2);
        exit(-1);
    }

    /* Read from the second file and write to the first file */
    while ((n = read(file2, buf, BUFFSIZE)) > 0) {
        if (write(file1, buf, n) != n) {
            printf("Error writing to output file\n");
            close(file2);
            close(file1);
            exit(-1);
        }
    }

    if (n < 0) {
        printf("Error reading input file\n");
        close(file2);
        close(file1);
        exit(-1);
    }

    close(file2);
    close(file1);

    return 0;
}
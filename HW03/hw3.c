/*
Name: Krish Wadhwani
BlazerId: KMW2
Project #: HW 3
To compile: make        (or: gcc -Wall -Wextra -std=c99 -o hw3 hw3.c)
To run: ./hw3 /path/to/directory
        ./hw3 -u username /path/to/directory   (graduate option: only files owned by username)
*/

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_BUFFER_SIZE 4096

/* Returns 1 if the file name ends in ".txt", otherwise 0. */
static int is_text_file(const char *name)
{
    size_t length = strlen(name);
    return length > 4 && strcmp(name + length - 4, ".txt") == 0;
}

/*
 * Counts whitespace-separated words in the file at path using read().
 * Reads in fixed-size chunks so very large files never need to fit in memory.
 * Returns 0 on success (word count stored in *words), -1 on failure (errno set).
 */
static int count_words(const char *path, unsigned long long *words)
{
    char buffer[READ_BUFFER_SIZE];
    ssize_t bytes_read;
    int in_word = 0;
    int fd = open(path, O_RDONLY);

    if (fd < 0)
        return -1;

    *words = 0;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        ssize_t i;
        for (i = 0; i < bytes_read; ++i) {
            if (isspace((unsigned char)buffer[i])) {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                (*words)++;
            }
        }
    }

    if (bytes_read < 0) {
        int saved_errno = errno;
        close(fd);
        errno = saved_errno;
        return -1;
    }

    close(fd);
    return 0;
}

/*
 * Work done by each child process: print the file name, size, and (for .txt
 * files) the word count. Never returns; the child exits with _exit().
 * owner_name is NULL unless the -u option is active.
 */
static void child_task(const char *name, const char *path, const struct stat *info,
                       const char *owner_name)
{
    char words_text[64];
    char owner_text[256] = "";

    if (is_text_file(name)) {
        unsigned long long words;
        if (count_words(path, &words) != 0) {
            fprintf(stderr, "Error: cannot read '%s': %s\n", name, strerror(errno));
            _exit(EXIT_FAILURE);
        }
        snprintf(words_text, sizeof(words_text), "%llu", words);
    } else {
        /* Word counting applies to .txt files only. */
        snprintf(words_text, sizeof(words_text), "N/A");
    }

    if (owner_name != NULL)
        snprintf(owner_text, sizeof(owner_text), " | Owner: %s", owner_name);

    /* One printf per file keeps each child's line from interleaving with others. */
    printf("File: %s | Size: %lld bytes | Words: %s%s\n",
           name, (long long)info->st_size, words_text, owner_text);
    fflush(stdout);
    _exit(EXIT_SUCCESS);
}

/* Waits for every outstanding child. Returns the number of children that failed. */
static int wait_for_children(void)
{
    int status;
    int failures = 0;
    pid_t pid;

    while ((pid = wait(&status)) > 0) {
        if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
            failures++;
    }
    /* wait() returns -1 with ECHILD once there are no children left. */
    return failures;
}

static void print_usage(void)
{
    fprintf(stderr, "Usage: ./hw3 <directory_name>\n");
    fprintf(stderr, "       ./hw3 -u <username> <directory_name>\n");
}

int main(int argc, char *argv[])
{
    const char *directory = NULL;
    const char *username = NULL;
    uid_t filter_uid = 0;
    DIR *dir;
    struct dirent *entry;
    int files_found = 0;
    int failures;
    int i;

    /* Parse arguments: optional "-u <username>" followed by the directory. */
    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-u") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -u requires a username.\n");
                print_usage();
                return EXIT_FAILURE;
            }
            username = argv[++i];
        } else if (directory == NULL) {
            directory = argv[i];
        } else {
            fprintf(stderr, "Error: too many arguments.\n");
            print_usage();
            return EXIT_FAILURE;
        }
    }

    if (directory == NULL) {
        print_usage();
        return EXIT_FAILURE;
    }

    /* Resolve the username to a UID once, up front. */
    if (username != NULL) {
        struct passwd *user = getpwnam(username);
        if (user == NULL) {
            fprintf(stderr, "Error: user '%s' not found.\n", username);
            return EXIT_FAILURE;
        }
        filter_uid = user->pw_uid;
    }

    dir = opendir(directory);
    if (dir == NULL) {
        if (errno == ENOENT || errno == ENOTDIR)
            fprintf(stderr, "Error: Directory not found.\n");
        else
            fprintf(stderr, "Error: cannot open directory '%s': %s\n", directory, strerror(errno));
        return EXIT_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[PATH_MAX];
        struct stat info;
        pid_t pid;
        int written;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        written = snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
        if (written < 0 || (size_t)written >= sizeof(path)) {
            fprintf(stderr, "Error: path too long: %s/%s\n", directory, entry->d_name);
            continue;
        }

        /* lstat so symbolic links are not followed; failures are reported, not fatal. */
        if (lstat(path, &info) != 0) {
            fprintf(stderr, "Error: cannot access '%s': %s\n", entry->d_name, strerror(errno));
            continue;
        }

        /* Only regular files are processed; subdirectories (and others) are skipped. */
        if (!S_ISREG(info.st_mode))
            continue;

        /* Graduate option: skip files not owned by the requested user. */
        if (username != NULL && info.st_uid != filter_uid)
            continue;

        files_found++;

        /* Flush so the child does not inherit and re-print buffered parent output. */
        fflush(stdout);
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Error: fork failed for '%s': %s\n", entry->d_name, strerror(errno));
            closedir(dir);
            wait_for_children();    /* do not leave already-created children orphaned */
            return EXIT_FAILURE;
        }
        if (pid == 0) {
            closedir(dir);
            child_task(entry->d_name, path, &info, username);
        }
        /* Parent continues the loop to create the next child. */
    }

    closedir(dir);

    /* The parent waits for all children before exiting. */
    failures = wait_for_children();

    if (files_found == 0)
        printf("No files found in '%s'.\n", directory);

    return failures > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
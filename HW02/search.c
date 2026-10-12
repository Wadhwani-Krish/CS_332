/*
Name: Krish Wadhwani
BlazerId: KMW2
Project #: HW 2
*/

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

typedef int FilterFunction(const char *path, const char *name, const struct stat *info, int depth);

typedef struct {
    bool show_details;
    bool use_size;
    off_t max_size;
    bool use_pattern;
    const char *pattern;
    int max_depth;
    char type;                 /* '\0' = any, 'f' = regular files, 'd' = directories */
} SearchOptions;

static SearchOptions options;

/* Function-pointer predicates used to filter entries. */
static int size_matches(const char *path, const char *name, const struct stat *info, int depth)
{
    (void)path; (void)name; (void)depth;
    /* Treat directories as size 0, matching the assignment specification. */
    return (S_ISDIR(info->st_mode) ? 0 : info->st_size) <= options.max_size;
}

static int pattern_matches(const char *path, const char *name, const struct stat *info, int depth)
{
    (void)path; (void)info;
    return strstr(name, options.pattern) != NULL && depth <= options.max_depth;
}

static int type_matches(const char *path, const char *name, const struct stat *info, int depth)
{
    (void)path; (void)name; (void)depth;
    if (options.type == 'f')
        return S_ISREG(info->st_mode);
    if (options.type == 'd')
        return S_ISDIR(info->st_mode);
    return 1;
}

static int passes_filters(const char *path, const char *name, const struct stat *info, int depth)
{
    FilterFunction *filters[3];
    size_t count = 0, i;

    if (options.use_size)
        filters[count++] = size_matches;
    if (options.use_pattern)
        filters[count++] = pattern_matches;
    if (options.type != '\0')
        filters[count++] = type_matches;

    for (i = 0; i < count; ++i) {
        if (!filters[i](path, name, info, depth))
            return 0;
    }
    return 1;
}

static void print_permissions(mode_t mode, char result[11])
{
    result[0] = S_ISDIR(mode) ? 'd' :
                S_ISLNK(mode) ? 'l' :
                S_ISREG(mode) ? '-' : '?';
    result[1] = (mode & S_IRUSR) ? 'r' : '-';
    result[2] = (mode & S_IWUSR) ? 'w' : '-';
    result[3] = (mode & S_IXUSR) ? 'x' : '-';
    result[4] = (mode & S_IRGRP) ? 'r' : '-';
    result[5] = (mode & S_IWGRP) ? 'w' : '-';
    result[6] = (mode & S_IXGRP) ? 'x' : '-';
    result[7] = (mode & S_IROTH) ? 'r' : '-';
    result[8] = (mode & S_IWOTH) ? 'w' : '-';
    result[9] = (mode & S_IXOTH) ? 'x' : '-';
    result[10] = '\0';
}

static void print_entry(const char *name, const char *path, const struct stat *info, int depth)
{
    int i;
    for (i = 0; i < depth; ++i)
        printf("\t");

    printf("%s", name);

    if (S_ISLNK(info->st_mode)) {
        char target[PATH_MAX + 1];
        ssize_t length = readlink(path, target, sizeof(target) - 1);
        if (length >= 0) {
            target[length] = '\0';
            printf(" (%s)", target);
        } else {
            printf(" (unavailable)");
        }
    }

    if (options.show_details) {
        char permissions[11];
        char time_text[64];
        struct tm *time_info = localtime(&info->st_atime);
        if (time_info != NULL)
            strftime(time_text, sizeof(time_text), "%Y-%m-%d %H:%M:%S", time_info);
        else
            strcpy(time_text, "unknown");

        print_permissions(info->st_mode, permissions);
        printf(" (size: %lld, permissions: %s, last access: %s)", (long long)(S_ISDIR(info->st_mode) ? 0 : info->st_size), permissions, time_text);
    }
    putchar('\n');
}

static int compare_names(const void *left, const void *right)
{
    const char *const *a = left;
    const char *const *b = right;
    return strcasecmp(*a, *b);
}

/* Recursively visit a directory. depth is relative to the starting directory. */
static void walk_directory(const char *directory, int depth)
{
    DIR *dir;
    struct dirent *entry;
    char **names = NULL;
    size_t count = 0, capacity = 0, i;

    dir = opendir(directory);
    if (dir == NULL) {
        fprintf(stderr, "search: cannot open '%s': %s\n",
                directory, strerror(errno));
        return;
    }

    /* Read names first so output is stable and easy to follow. */
    while ((entry = readdir(dir)) != NULL) {
        char *copy;
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        if (count == capacity) {
            size_t new_capacity = capacity == 0 ? 16 : capacity * 2;
            char **new_names = realloc(names, new_capacity * sizeof(*names));
            if (new_names == NULL) {
                fprintf(stderr, "search: out of memory\n");
                closedir(dir);
                for (i = 0; i < count; ++i) free(names[i]);
                free(names);
                return;
            }
            names = new_names;
            capacity = new_capacity;
        }
        copy = strdup(entry->d_name);
        if (copy == NULL) {
            fprintf(stderr, "search: out of memory\n");
            closedir(dir);
            for (i = 0; i < count; ++i) free(names[i]);
            free(names);
            return;
        }
        names[count++] = copy;
    }
    closedir(dir);
    qsort(names, count, sizeof(*names), compare_names);

    for (i = 0; i < count; ++i) {
        char path[PATH_MAX];
        struct stat info;
        int written = snprintf(path, sizeof(path), "%s/%s", directory, names[i]);

        if (written < 0 || (size_t)written >= sizeof(path)) {
            fprintf(stderr, "search: path too long: %s/%s\n", directory, names[i]);
            free(names[i]);
            continue;
        }

        /* lstat reports symbolic links themselves rather than their targets. */
        if (lstat(path, &info) != 0) {
            fprintf(stderr, "search: cannot inspect '%s': %s\n", path, strerror(errno));
            free(names[i]);
            continue;
        }

        if (passes_filters(path, names[i], &info, depth + 1))
            print_entry(names[i], path, &info, depth + 1);

        /* Do not follow symbolic links, avoiding loops through linked directories. */
        if (S_ISDIR(info.st_mode))
            walk_directory(path, depth + 1);

        free(names[i]);
    }
    free(names);
}

static void usage(const char *program)
{
    fprintf(stderr,
        "Usage: %s [-S] [-s bytes] [-f pattern depth] [-t f|d] [directory]\n"
        "  -S          show size, permissions, and last access time\n"
        "  -s bytes    show entries whose size is <= bytes\n"
        "  -f pattern depth  match names containing pattern at depth <= depth\n"
        "  -t f        regular files only (graduate option)\n"
        "  -t d        directories only (graduate option)\n",
        program);
}

int main(int argc, char *argv[])
{
    int option;
    char *end = NULL;
    bool size_seen = false, pattern_seen = false, type_seen = false;
    const char *start_directory = ".";
    struct stat root_info;

    memset(&options, 0, sizeof(options));

    /* Leading '+' makes getopt stop at the first non-option argument. */
    while ((option = getopt(argc, argv, "+Ss:f:t:")) != -1) {
        switch (option) {
        case 'S':
            options.show_details = true;
            break;
        case 's':
            errno = 0;
            options.max_size = (off_t)strtoll(optarg, &end, 10);
            if (errno != 0 || end == optarg || *end != '\0' || options.max_size < 0) {
                fprintf(stderr, "search: invalid size: %s\n", optarg);
                return EXIT_FAILURE;
            }
            options.use_size = true;
            size_seen = true;
            break;
        case 'f': {
            long parsed_depth;
            if (optind >= argc) {
                fprintf(stderr, "search: -f requires a pattern and a depth\n");
                usage(argv[0]);
                return EXIT_FAILURE;
            }
            options.pattern = optarg;
            errno = 0;
            parsed_depth = strtol(argv[optind++], &end, 10);
            if (errno != 0 || end == argv[optind - 1] || *end != '\0' || parsed_depth < 0 || parsed_depth > INT_MAX) {
                fprintf(stderr, "search: invalid depth\n");
                return EXIT_FAILURE;
            }
            options.max_depth = (int)parsed_depth;
            options.use_pattern = true;
            pattern_seen = true;
            break;
        }
        case 't':
            if (strcmp(optarg, "f") != 0 && strcmp(optarg, "d") != 0) {
                fprintf(stderr, "search: -t must be followed by f or d\n");
                return EXIT_FAILURE;
            }
            options.type = optarg[0];
            type_seen = true;
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    (void)size_seen;
    (void)pattern_seen;
    (void)type_seen;

    if (optind < argc) {
        start_directory = argv[optind++];
        if (optind < argc) {
            fprintf(stderr, "search: unexpected argument: %s\n", argv[optind]);
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (lstat(start_directory, &root_info) != 0) {
        fprintf(stderr, "search: cannot access '%s': %s\n", start_directory, strerror(errno));
        return EXIT_FAILURE;
    }
    if (!S_ISDIR(root_info.st_mode)) {
        fprintf(stderr, "search: '%s' is not a directory\n", start_directory);
        return EXIT_FAILURE;
    }

    /* Print the starting directory as the hierarchy root. */
    printf("%s\n", start_directory);
    walk_directory(start_directory, 0);
    return EXIT_SUCCESS;
}

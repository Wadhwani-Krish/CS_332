#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

char *filetype(unsigned char type) {
    char *str;

    switch(type) {
        case DT_BLK:
            str = "block device";
            break;
        case DT_CHR:
            str = "character device";
            break;
        case DT_DIR:
            str = "directory";
            break;
        case DT_FIFO:
            str = "named pipe (FIFO)";
            break;
        case DT_LNK:
            str = "symbolic link";
            break;
        case DT_REG:
            str = "regular file";
            break;
        case DT_SOCK:
            str = "UNIX domain socket";
            break;
        case DT_UNKNOWN:
            str = "unknown file type";
            break;
        default:
            str = "UNKNOWN";
    }

    return str;
}


 //Recursively traverse a directory
void traverseDirectory(char *path, int level) {

    DIR *parentDir;
    struct dirent *dirent;

    parentDir = opendir(path);

    if (parentDir == NULL) {
        printf("Error opening directory '%s'\n", path);
        return;
    }

    while ((dirent = readdir(parentDir)) != NULL) {

        // Ignore . and ..
        if (strcmp(dirent->d_name, ".") == 0 ||
            strcmp(dirent->d_name, "..") == 0) {
            continue;
        }

        //Print indentation based on directory level 
        for (int i = 0; i < level; i++) {
            printf("    ");
        }

        printf("%s (%s)\n",
               dirent->d_name,
               filetype(dirent->d_type));

        
        //If the entry is a directory,recursively traverse it.
        
        if (dirent->d_type == DT_DIR) {

            char newPath[1024];

            snprintf(newPath, sizeof(newPath),
                     "%s/%s", path, dirent->d_name);

            traverseDirectory(newPath, level + 1);
        }
    }

    closedir(parentDir);
}

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage: %s <dirname>\n", argv[0]);
        exit(-1);
    }

    // Start recursive traversal 
    traverseDirectory(argv[1], 0);

    return 0;
}
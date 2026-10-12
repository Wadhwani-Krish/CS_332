# Search

A C program that recursively traverses a directory hierarchy and lists files and subdirectories. It also supports filtering by file size, filename pattern, depth, and file type.

## Requirements

* GCC compiler

## Compile

```bash
make
```

Or compile directly:

```bash
gcc -Wall -Wextra -std=c11 search.c -o search
```

## Run

```bash
./search
or
./search <directory>
```

Examples:

```bash
./search projects
./search -S projects
./search -s 1024 projects
./search -f jpg 1 projects
./search -f jpg 1 -s 1024 projects
./search -S -f jpg 2 -s 1024 projects
```

## Example

```text
projects
    fread.c
    fwrite.c
    project1
        project1.docx
        README
    project2
        project2.docx
        README
```

The order of the entries may differ. Each level of the directory hierarchy is indented with tabs.

## How It Works

The program uses `opendir()`, `readdir()`, and `closedir()` to open, read, and close directories. It uses `lstat()` to obtain information about files and directories and `readlink()` to display the target of a symbolic link.

A recursive function called `walk_directory()` visits each directory. When the program finds a subdirectory, it creates the path to that directory and calls `walk_directory()` again. This allows the program to traverse all levels of the directory hierarchy.

The program uses function pointers to apply the requested filters. The `-S` option displays file size, permissions, and last access time. The `-s` option lists entries whose size is less than or equal to the specified value. The `-f` option lists entries whose names contain a specified substring and whose depth is less than or equal to the given value. The graduate `-t` option filters regular files or directories.

When multiple filtering options are used, an entry must satisfy all the selected filters. Symbolic links are displayed with the file name they point to in parentheses. The program ignores the `.` and `..` directory entries to prevent unnecessary traversal of the current and parent directories.

## Testing

The program was compiled using GCC and tested with the provided `projects` directory structure.

The following options were tested:

* Basic directory traversal.
* Displaying file details using `-S`.
* Filtering entries by size using `-s`.
* Filtering entries by filename pattern and depth using `-f`.
* Combining the `-s` and `-f` options.

Additional tests should include symbolic links and the graduate `-t f` and `-t d` options, if required.

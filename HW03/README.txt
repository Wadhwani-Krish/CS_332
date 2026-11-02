# File Info with fork and wait

A C program that takes a directory, creates a child process with `fork()` for each regular file in it, and has each child print the file's name, size, and word count (.txt files only). The parent waits for all children before exiting. The graduate option `-u <username>` lists only files owned by that user.

## Requirements

- GCC compiler
- A UNIX/Linux system

## Files

- `hw3.c` - Source code
- `Makefile` - Builds the program
- `README.txt` - This file

## Compile

```bash
make
```

## Run

```bash
./hw3 <directory_name>
./hw3 -u <username> <directory_name>
```

## Example

```bash
./hw3 /path/to/directory
```

```text
File: file1.txt | Size: 1234 bytes | Words: 200
File: image.png | Size: 345 bytes | Words: N/A
```

Errors:

```text
$ ./hw3
Usage: ./hw3 <directory_name>

$ ./hw3 /no/such/dir
Error: Directory not found.
```

Output order can vary because the children run at the same time.

## How It Works

The parent reads the directory with `readdir()`, uses `lstat()` to skip anything that is not a regular file, and calls `fork()` for each file. Each child prints the name, the size from `st_size`, and the word count (counted with `read()`). The parent calls `wait()` until all children are done.

## Error Handling

- No argument: prints the usage message.
- Invalid directory: prints `Error: Directory not found.`
- `fork()` failure: prints an error and exits.
- Unreadable file: the child prints an error message.
- Empty files report 0 words; an empty directory prints `No files found`.
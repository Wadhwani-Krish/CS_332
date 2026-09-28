# String Length Comparison

A C program that recursively traverses a directory hierarchy and lists all files and subdirectories, including the type of each file.
## Requirements

- GCC compiler

## Compile

```bash
gcc -Wall lab5.c -o lab5
```

## Run

```bash
./lab5 <directory>
or 
./lab5 ./test
```

## Example

```text
test/ 
folder1 (directory)
    folder2 (directory)
        file3 (regular file)
    file2 (regular file)
file1.txt (regular file)
```

## How It Works

The program uses opendir(), readdir(), and closedir() to open, read, and close directories.

A recursive function called traverseDirectory() is used to visit each directory. When the program finds a subdirectory, it creates the path to that directory and calls traverseDirectory() again. This allows the program to continue through all levels of the directory hierarchy.

The program also uses the filetype() function to identify and display the type of each file or directory.

The . and .. directory entries are ignored to prevent the program from repeatedly traversing the same directories.
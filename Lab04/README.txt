# File Append Program

A simple C program that reads the contents of one file and appends them to another file.

## Requirements

* GCC compiler

## Compile

```bash
gcc -Wall -o lab04 lab04.c
```

## Run

```bash
./lab04 file1 file2
```

## Example

Suppose `file1` contains:

```text
Hello World!
```

And `file2` contains:

```text
This is a test.
```

Run:

```bash
./lab04 file1 file2
```

After running the program, `file1` will contain:

```text
Hello World!
This is a test.
```

The contents of `file2` are appended to the end of `file1`. The source file is not changed.

## How It Works

The program uses Linux file system system calls to copy data from one file to another.

* `open()` opens the source and destination files.
* `read()` reads data from the source file into a buffer.
* `write()` writes the data to the destination file.
* `close()` closes both files when the operation is finished.
* `strcmp()` checks that the two filenames are different.
* A buffer of `4096` bytes is used to process the file data.

The destination file is opened with `O_APPEND`, which causes the source file's contents to be added to the end of the destination file.

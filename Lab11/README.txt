# Lab 11 - Pipes and Process Communication

A C program that prompts the user to enter UNIX commands and executes them using `popen()` and `pclose()`. The program displays command output and continues accepting commands until the user enters `quit`.

## Requirements

- Linux or a UNIX-like operating system
- GCC compiler

## Compile

```bash
gcc -Wall lab11.c -o lab11
```

## Run

```bash
./lab11
```

## Example

```text
Enter command: ls *.c
lab11.c pipe1.c pipe2.c
Enter command: ls -l
total 12
-rwxr-xr-x 1 user user  ... lab11
-rw-r--r-- 1 user user  ... lab11.c
Enter command: ls *.c | sort
lab11.c
pipe1.c
pipe2.c
Enter command: quit
Exiting program...bye!
```

## How It Works

1. The program prompts the user to enter a UNIX command.
2. `getline()` reads the entire command, including commands with spaces and shell special characters.
3. `strcmp()` checks whether the user entered `quit`.
4. `popen(command, "r")` executes the command through a shell and opens a pipe so the program can read its standard output.
5. `fgets()` reads the output, and `fputs()` displays it in the terminal.
6. `pclose()` closes the pipe and waits for the command to finish.
7. The loop repeats until the user enters `quit` or reaches the end of input.
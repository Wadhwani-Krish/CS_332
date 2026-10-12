# Process Execution and Timing

A C program that reads commands from an input file, creates a child process for each command using `fork()`, executes the command and its arguments with `execvp()`, waits for the child process to finish, and records the command's start and end times in a log file.

## Requirements

- GCC compiler
- An input text file containing one command per line

## Files

- `lab7.c` - Source code for the program
- `commands.txt` - Example input file containing commands to execute
- `output.log` - Log file created by the program with each command and its start and end times

## Compile

```bash
gcc -Wall -O -o lab7 lab7.c
```

## Run

```bash
./lab7 commands.txt
```

Replace `commands.txt` with the name of your input file.

## Example

Input file (`commands.txt`):

```text
uname -a
ls -l
pwd
```

After running the program, it creates `output.log`. Each line contains the command and its start and end times, separated by tabs.

Example log format:

```text
uname -a    Sun Oct 11 21:00:00 2026    Sun Oct 11 21:00:01 2026
ls -l       Sun Oct 11 21:00:01 2026    Sun Oct 11 21:00:01 2026
pwd         Sun Oct 11 21:00:01 2026    Sun Oct 11 21:00:01 2026
```

The timestamps above are examples; the actual output depends on when the program is run.

## How It Works

The program opens the input file using `fopen()` and reads commands one line at a time with `fgets()`.

The `strtok()` function separates each command into the executable name and its arguments. The program records the start time with `time()` and creates a child process using `fork()`.

The child process calls `execvp()` to run the command. The parent process uses `waitpid()` to wait for the child to finish, then records the end time.

Finally, `fprintf()` writes the original command and its start and end times to `output.log`, with the fields separated by tabs. The program closes both files when it is finished.
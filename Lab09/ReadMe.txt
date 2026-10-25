# Signal Handling with fork, execvp, and waitpid
 
A C program that creates a child process with `fork()`, runs a command given on the command line using `execvp()`, and waits for it with `waitpid()`. Control-C and Control-Z affect only the child process, and the parent keeps waiting until it receives a quit signal (Control-\).
 
## Requirements
 
- GCC compiler
- A UNIX/Linux system (tested with signals from the terminal)
 
## Files
 
- `forkexecvp.c` - Source code for the program
- `README.txt` - This file
 
## Compile
 
```bash
gcc -Wall -o forkexecvp forkexecvp.c
```
 
## Run
 
```bash
./forkexecvp <command> [args]
```
 
Replace `<command> [args]` with the command you want the child process to run, for example `sleep 100`.
 
## Example
 
```bash
./forkexecvp sleep 100
```
 
While the program is running:
 
- Press `Control-Z` to suspend the child process. The parent keeps waiting.
- Press `Control-C` to interrupt the child process. The parent keeps waiting.
- Press `Control-\` to make the parent quit.
 
Example output:
 
```text
Wait for the child process (pid 12345) to terminate
Ctrl-C / Ctrl-Z affect the child; press Ctrl-\ to make the parent quit
^Z
[parent] caught SIGTSTP (Ctrl-Z): child suspended, parent still waiting
Child process was stopped by signal 20 (Stopped)
^C
[parent] caught SIGINT (Ctrl-C): child interrupted, parent still waiting
^\
[parent] caught SIGQUIT (Ctrl-\): parent will quit
Child process (pid 12345) was killed by the parent before quitting
[12340]: Exiting program .....
```
 
The process IDs above are examples; the actual output depends on the system. If the child is stopped and you then press `Control-C`, the signal stays pending until the child is continued (for example, with `kill -CONT <pid>` from another terminal).
 
## How It Works
 
The terminal sends keyboard signals to every process in the foreground process group, so both the parent and the child receive `Control-C`, `Control-Z`, and `Control-\`.
 
The parent installs handlers for `SIGINT`, `SIGTSTP`, and `SIGQUIT` using `sigaction()`, so it is not terminated or stopped by `Control-C` or `Control-Z`. The child never installs handlers, and `execvp()` resets caught signals to their default actions, so `SIGINT` terminates the child and `SIGTSTP` stops it.
 
The parent uses `waitpid()` with `WUNTRACED | WCONTINUED` so it is notified when the child exits, is terminated by a signal, is stopped, or is continued. The handlers are installed without `SA_RESTART`, so `waitpid()` returns with `EINTR` when a signal arrives and the parent can check its quit flag. The handler uses only `write()`, which is safe to call in a signal handler.
 
After the child is gone, the parent sleeps with `sigsuspend()` until `SIGQUIT` (`Control-\`) arrives. If the child is still alive when the quit signal is received (for example, it is stopped), the parent kills and reaps it with `kill()` and `waitpid()` before exiting, so no orphan or zombie process is left behind.
 
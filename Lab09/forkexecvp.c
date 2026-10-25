#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
 
static volatile sig_atomic_t quit_flag = 0;
 
/* Parent's signal handler. Only async-signal-safe functions (write) are used. */
static void sig_handler(int signo) {
    const char *msg;
 
    switch (signo) {
        case SIGINT:
            msg = "\n[parent] caught SIGINT (Ctrl-C): child interrupted, parent still waiting\n";
            write(STDOUT_FILENO, msg, strlen(msg));
            break;
        case SIGTSTP:
            msg = "\n[parent] caught SIGTSTP (Ctrl-Z): child suspended, parent still waiting\n";
            write(STDOUT_FILENO, msg, strlen(msg));
            break;
        case SIGQUIT:
            msg = "\n[parent] caught SIGQUIT (Ctrl-\\): parent will quit\n";
            write(STDOUT_FILENO, msg, strlen(msg));
            quit_flag = 1;
            break;
        default:
            break;
    }
}
 
/* Install handler with sigaction. SA_RESTART is intentionally NOT set so that
 * waitpid() returns with EINTR when a signal arrives. */
static void install_handler(int signo) {
    struct sigaction sa;
 
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(signo, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}
 
int main(int argc, char **argv) {
    pid_t pid;
    int status;
    int child_done = 0;
 
    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        exit(-1);
    }
 
    pid = fork();
    if (pid == 0) { /* this is child process */
        /* Handlers are not installed in the child, so it keeps the default
         * action for SIGINT/SIGTSTP/SIGQUIT (terminate/stop/terminate). */
        execvp(argv[1], &argv[1]);
        printf("If you see this statement then execvp failed ;-(\n");
        perror("execvp");
        exit(-1);
    } else if (pid > 0) { /* this is the parent process */
        sigset_t block_mask, old_mask;
 
        install_handler(SIGINT);
        install_handler(SIGTSTP);
        install_handler(SIGQUIT);
 
        printf("Wait for the child process (pid %ld) to terminate\n", (long)pid);
        printf("Ctrl-C / Ctrl-Z affect the child; press Ctrl-\\ to make the parent quit\n");
 
        /* Phase 1: wait for the child; keep going until SIGQUIT is received. */
        while (!quit_flag) {
            if (!child_done) {
                /* WUNTRACED: also return when the child is stopped (Ctrl-Z)
                 * WCONTINUED: also return when the child is resumed (SIGCONT) */
                pid_t r = waitpid(pid, &status, WUNTRACED | WCONTINUED);
                if (r == -1) {
                    if (errno == EINTR)
                        continue; /* interrupted by a signal, re-check quit_flag */
                    perror("waitpid");
                    break;
                }
                if (WIFEXITED(status)) {
                    printf("Child process exited with status = %d\n", WEXITSTATUS(status));
                    child_done = 1;
                } else if (WIFSIGNALED(status)) {
                    printf("Child process was terminated by signal %d (%s)\n",
                           WTERMSIG(status), strsignal(WTERMSIG(status)));
                    child_done = 1;
                } else if (WIFSTOPPED(status)) {
                    printf("Child process was stopped by signal %d (%s)\n",
                           WSTOPSIG(status), strsignal(WSTOPSIG(status)));
                } else if (WIFCONTINUED(status)) {
                    printf("Child process was continued\n");
                }
            } else {
                /* Child is gone; sleep until SIGQUIT arrives. Block SIGQUIT
                 * while checking the flag, then atomically unblock+sleep with
                 * sigsuspend so the signal cannot be missed. */
                sigemptyset(&block_mask);
                sigaddset(&block_mask, SIGQUIT);
                sigprocmask(SIG_BLOCK, &block_mask, &old_mask);
                while (!quit_flag)
                    sigsuspend(&old_mask);
                sigprocmask(SIG_SETMASK, &old_mask, NULL);
            }
        }
 
        /* Quit signal received. If the child is still around (e.g., it is
         * stopped), kill it and reap it so no orphan/zombie is left behind. */
        if (!child_done) {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            printf("Child process (pid %ld) was killed by the parent before quitting\n", (long)pid);
        }
    } else { /* we have an error */
        perror("fork"); /* use perror to print the system error message */
        exit(EXIT_FAILURE);
    }
 
    printf("[%ld]: Exiting program .....\n", (long)getpid());
 
    return 0;
}
 
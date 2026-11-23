#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define PRODUCERS 10
#define CONSUMERS 20
#define NUMBERS 500
#define READ_NUMBERS 250

int pipe_fd[2];

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t write_sem;

/* Structure for producer thread information */
typedef struct {
    int id;
} Producer;

/* Structure for consumer thread information */
typedef struct {
    int id;
    long sum;
} Consumer;

/* Write all bytes to the pipe */
void write_data(int fd, int number)
{
    char *ptr = (char *)&number;
    int remaining = sizeof(int);

    while (remaining > 0) {
        int result = write(fd, ptr, remaining);

        if (result < 0) {
            if (errno == EINTR)
                continue;

            perror("Write error");
            exit(1);
        }

        ptr += result;
        remaining -= result;
    }
}

/* Read one complete integer from the pipe */
int read_data(int fd, int *number)
{
    char *ptr = (char *)number;
    int remaining = sizeof(int);

    while (remaining > 0) {
        int result = read(fd, ptr, remaining);

        if (result == 0)
            return 0;

        if (result < 0) {
            if (errno == EINTR)
                continue;

            perror("Read error");
            return -1;
        }

        ptr += result;
        remaining -= result;
    }

    return 1;
}

/* Producer thread function */
void *producer(void *arg)
{
    Producer *p = (Producer *)arg;

    int used[1001] = {0};

    unsigned int seed = time(NULL) + p->id * 100;

    for (int i = 0; i < NUMBERS; i++) {

        int number;

        /* Generate a unique number for this producer */
        do {
            number = rand_r(&seed) % 1001;
        } while (used[number]);

        used[number] = 1;

        /* Synchronize access to the pipe */
        sem_wait(&write_sem);

        write_data(pipe_fd[1], number);

        sem_post(&write_sem);

        /* Print progress without mixing thread output */
        if ((i + 1) % 50 == 0) {

            pthread_mutex_lock(&print_mutex);

            printf("Producer %d (Thread ID: %lu): %d%% complete\n",
                   p->id,
                   (unsigned long)pthread_self(),
                   (i + 1) * 100 / NUMBERS);

            pthread_mutex_unlock(&print_mutex);
        }
    }

    pthread_mutex_lock(&print_mutex);

    printf("Producer %d (Thread ID: %lu) has finished!\n",
           p->id,
           (unsigned long)pthread_self());

    pthread_mutex_unlock(&print_mutex);

    return NULL;
}

/* Consumer thread function */
void *consumer(void *arg)
{
    Consumer *c = (Consumer *)arg;

    c->sum = 0;

    for (int i = 0; i < READ_NUMBERS; i++) {

        int number;

        /*
         * Only one consumer reads an integer at a time.
         * This prevents consumers from interfering with each other.
         */
        pthread_mutex_lock(&read_mutex);

        int result = read_data(pipe_fd[0], &number);

        pthread_mutex_unlock(&read_mutex);

        if (result != 1) {
            fprintf(stderr, "Consumer %d: Pipe read failed.\n", c->id);
            return NULL;
        }

        c->sum += number;
    }

    printf("Consumer %d finished. Sum = %ld\n",
           c->id, c->sum);

    return NULL;
}

/* Child process function */
void child_process()
{
    pthread_t threads[CONSUMERS];
    Consumer consumers[CONSUMERS];

    /* Child does not write to the pipe */
    close(pipe_fd[1]);

    printf("Child process started. Creating consumer threads...\n");

    /* Create 20 consumer threads */
    for (int i = 0; i < CONSUMERS; i++) {

        consumers[i].id = i + 1;
        consumers[i].sum = 0;

        if (pthread_create(&threads[i], NULL,
                           consumer, &consumers[i]) != 0) {
            perror("Consumer thread creation failed");
            exit(1);
        }
    }

    /* Wait for all consumer threads */
    for (int i = 0; i < CONSUMERS; i++) {
        pthread_join(threads[i], NULL);
    }

    close(pipe_fd[0]);

    /* Calculate average of all consumer sums */
    long total = 0;

    for (int i = 0; i < CONSUMERS; i++) {
        total += consumers[i].sum;
    }

    double average = (double)total / CONSUMERS;

    /*
     * Redirect standard output to a text file.
     * printf() will now write to average.txt.
     */
    if (freopen("average.txt", "w", stdout) == NULL) {
        perror("File redirection failed");
        exit(1);
    }

    printf("Average of Consumer Thread Sums: %.2f\n", average);

    printf("Total numbers processed: %d\n",
           PRODUCERS * NUMBERS);

    printf("Total sum: %ld\n", total);

    exit(0);
}

/* Main function - Parent process */
int main()
{
    pthread_t producer_threads[PRODUCERS];
    Producer producers[PRODUCERS];

    printf("Starting Producer-Consumer Program\n");

    /* Create the pipe */
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    /* Create child process */
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    /* Child process */
    if (pid == 0) {
        child_process();
    }

    /* Parent process */

    close(pipe_fd[0]);

    /* Initialize semaphore */
    sem_init(&write_sem, 0, 1);

    printf("Parent process started. Creating producer threads...\n");

    /* Create 10 producer threads */
    for (int i = 0; i < PRODUCERS; i++) {

        producers[i].id = i + 1;

        if (pthread_create(&producer_threads[i], NULL,
                           producer, &producers[i]) != 0) {
            perror("Producer thread creation failed");
            exit(1);
        }
    }

    /* Wait for all producer threads */
    for (int i = 0; i < PRODUCERS; i++) {
        pthread_join(producer_threads[i], NULL);
    }

    printf("All producer threads have completed.\n");

    /*
     * Close the writing end.
     * This tells the child process that no more data is coming.
     */
    close(pipe_fd[1]);

    sem_destroy(&write_sem);

    /* Wait for child process */
    waitpid(pid, NULL, 0);

    pthread_mutex_destroy(&print_mutex);
    pthread_mutex_destroy(&read_mutex);

    printf("Parent process finished.\n");
    printf("Check average.txt for the final results.\n");

    return 0;
}
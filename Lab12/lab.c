#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* Structure to pass information to each thread */
typedef struct {
    double *a;
    double *sum;
    int N;
    int size;
    long tid;
} thread_data;

void *compute(void *arg) {

    thread_data *data = (thread_data *)arg;

    int myStart, myEnd, myN, i;

    /* Determine which elements this thread will calculate */
    myN = data->N / data->size;

    myStart = data->tid * myN;
    myEnd = myStart + myN;

    if (data->tid == (data->size - 1))
        myEnd = data->N;

    /* Calculate partial sum */
    double mysum = 0.0;

    for (i = myStart; i < myEnd; i++) {
        mysum += data->a[i];
    }

    /* Lock mutex before updating shared sum */
    pthread_mutex_lock(&mutex);

    *(data->sum) += mysum;

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {

    long i;
    pthread_t *tid;

    if (argc != 3) {
        printf("Usage: %s <# of elements> <# of threads>\n", argv[0]);
        exit(-1);
    }

    int N = atoi(argv[1]);
    int size = atoi(argv[2]);

    if (N <= 0 || size <= 0) {
        printf("Number of elements and threads must be positive.\n");
        return 1;
    }

    /* Allocate memory */
    tid = malloc(sizeof(pthread_t) * size);

    double *a = malloc(sizeof(double) * N);
    double sum = 0.0;

    thread_data *data = malloc(sizeof(thread_data) * size);

    if (tid == NULL || a == NULL || data == NULL) {
        printf("Memory allocation failed.\n");
        free(tid);
        free(a);
        free(data);
        return 1;
    }

    /* Initialize array */
    for (i = 0; i < N; i++) {
        a[i] = (double)(i + 1);
    }

    /* Create threads */
    for (i = 0; i < size; i++) {

        data[i].a = a;
        data[i].sum = &sum;
        data[i].N = N;
        data[i].size = size;
        data[i].tid = i;

        pthread_create(&tid[i], NULL, compute, &data[i]);
    }

    /* Wait for threads to finish */
    for (i = 0; i < size; i++) {
        pthread_join(tid[i], NULL);
    }

    /* Display result */
    printf("The total is %g, it should be equal to %g\n",
           sum, ((double)N * (N + 1)) / 2);

    /* Free allocated memory */
    free(tid);
    free(a);
    free(data);

    pthread_mutex_destroy(&mutex);

    return 0;
}
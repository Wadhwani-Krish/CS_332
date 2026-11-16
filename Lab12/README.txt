
# Lab 12 - POSIX Threads and Mutex Synchronization

A C program that uses POSIX threads (Pthreads) to calculate the sum of an array. Each thread calculates part of the array, and a mutex is used to safely update the shared total. A structure is passed to each thread to provide the information needed for the calculation.

## Requirements

- Linux or a UNIX-like operating system
- GCC compiler
- POSIX threads library (Pthreads)

## Compile

gcc -Wall pthread_sum.c -lpthread -o pthread_sum

## Run

./pthread_sum 1000 4

## Example

The following example calculates the sum of 1,000 array elements using 4 threads.

```text
The total is 500500, it should be equal to 500500
```

## How It Works

1. The program accepts the number of array elements and the number of threads as command-line arguments.
2. Memory is allocated for the array, thread IDs, and thread data structures.
3. A structure stores the array pointer, shared sum pointer, number of elements, number of threads, and thread ID.
4. Each thread receives its own structure as an argument through pthread_create().
5. Each thread calculates a partial sum of the array.
6. pthread_mutex_lock() locks the mutex before updating the shared sum.
7. pthread_mutex_unlock() releases the mutex after the update is complete.
8. pthread_join() waits for all threads to finish.
9. The program displays the calculated sum and compares it with the expected result.
10. Dynamically allocated memory is freed before the program exits.

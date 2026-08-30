#include <stdio.h>

int main() {
    int given_number; int i;
    int is_prime = 1;
    printf("Enter a number to determine if prime:\n");
    scanf("%d", &given_number);

    if (given_number <= 1) {
        is_prime = 0;
    } else {
        for (i = 2; i < given_number; i++) {
            if (given_number % i == 0) { // is the number divisible by i 
                is_prime = 0; // then false the number is not prime 
                break; /* to speed up the process does not have to check throug all the numbers. */
            }
        }
    }

    if (is_prime) {
        printf("The number is prime\n");
    } else {
        printf("The number is not prime\n");
    }

    return 0;
}
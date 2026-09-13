
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
char *readString();
void insertionSort(char *str);
void displayString(char *str);

int main() {

    // Read the string
    char *str = readString();

    // Display original string
    printf("Given string is: ");
    displayString(str);

    // Sort the characters
    insertionSort(str);

    // Display sorted string
    printf("Sorted string is: ");
    displayString(str);

    // Free dynamically allocated memory
    free(str);

    return 0;
}


// Function to read the string and dynamically allocate memory
char *readString() {

    char temp[1000];

    printf("Please enter a string: ");
    scanf("%999s", temp);

    // Determine length of the string
    int length = strlen(temp);

    // Allocate memory based on string length
    char *str = malloc((length + 1) * sizeof(char));

    if (str == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // Copy the string into dynamically allocated memory
    strcpy(str, temp);

    return str;
}


// Function to perform insertion sort on characters
void insertionSort(char *str) {

    int length = strlen(str);

    for (int i = 1; i < length; i++) {

        char key = str[i];
        int currLoc = i;

        // Compare characters
        while (currLoc > 0 && str[currLoc - 1] > key) {

            str[currLoc] = str[currLoc - 1];

            currLoc--;
        }

        // Put character in correct position
        str[currLoc] = key;
    }
}


// Function to display the string
void displayString(char *str) {

    printf("%s\n", str);
}
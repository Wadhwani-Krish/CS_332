#include <stdio.h>

int main() {
    int numElements;

    // Prompt user for number of elements
    printf("Please enter number of elements in array: ");
    scanf("%d", &numElements);

    int arr[numElements];

    // Read array elements
    for (int i = 0; i < numElements; i++) {
        printf("Please enter element %d of array: ", i + 1);
        scanf("%d", &arr[i]);
    }

    // Display given array
    printf("Given array is: [");
    for (int i = 0; i < numElements; i++) {
        printf("%d", arr[i]);

        if (i < numElements - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    // Insertion Sort
    for (int i = 1; i < numElements; i++) {
        int currLoc = i;

        while (currLoc > 0 && arr[currLoc - 1] > arr[currLoc]) {
            int temp = arr[currLoc];

            arr[currLoc] = arr[currLoc - 1];
            arr[currLoc - 1] = temp;

            currLoc--;
        }
    }

    // Display sorted array
    printf("Sorted array is: [");
    for (int i = 0; i < numElements; i++) {
        printf("%d", arr[i]);

        if (i < numElements - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    return 0;
}


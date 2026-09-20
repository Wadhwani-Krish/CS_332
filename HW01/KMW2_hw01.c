#include <stdio.h>
#include <math.h>
#include <stdbool.h>//for function 4 
// Function 1
int sumOfDigits(int n)
{
    // if the number is 0 or negative it will return -1
    if (n <= 0)
    {return -1;}
    
    int sum = 0;
     // have to break every digit and add it to sum 
     while (n > 0)
     {
        sum += n % 10;
        n /= 10;
     }

     return sum;
     
}

//Function 2

int UABMaxMinDiff(int arr[])
{
    int size = sizeof(arr)/ 2; //to get the size of arr we divide the memory size of arry by the size of one int or any element 
    // I think the compiler is automally converting int to short int so I had to divide it by 2
    int max = arr[0];
    int min = arr[0];
    //printf("Size of the arr is %d.\n", size);
    // we have to finnd the max and min first 
    for (int i = 1; i < size; i++){
        if (arr[i] > max){
            max = arr[i];
        }
        if (arr[i]< min)
        {
            min = arr[i];

        }
    }
    // get the difference and retun 
    return max - min;

}
//Function 3
void replaceEvenWithZero( int arr[])
{
    int size = size = sizeof(arr)/ 2; //Used the same from fun2
    for (int i = 0; i < size; i++){
        if (arr[i] % 2 ==0){
            arr[i] = 0;
        }
    }

}
//Function 4
bool perfectSquare(int n){  
    if (n <0){
        return false;
    }
    int root = (int)sqrt(n);
    return root * root ==n; 

}

//Function 5

int countVowels(char s[]){
    int count =0;
    
    for (int i = 0; s[i] != '\0'; i++)
    {
        if (s[i] == 'a' || s[i] == 'e' || s[i] == 'i' || s[i] == 'o' || s[i] == 'u' || s[i] == 'A' || s[i] == 'E' || s[i] == 'I' || s[i] == 'O' || s[i] == 'U')
        {
            count++;
        }
    }

    return count;
}

int main()
{
    // IMP Have to compile the code w/ -lm 
    //GCC isn't linking the math library




    printf("----- sumOfDigits(n) ----- \n");
    printf("  Input             Outputs \n");
    printf("  n = 123            %d\n", sumOfDigits(123));
    printf("  n = 405            %d\n", sumOfDigits(405));
    printf("  n = 0              %d\n", sumOfDigits(0));
    printf("  n = 7              %d\n", sumOfDigits(7));
    printf("  n = -308           %d\n", sumOfDigits(-308));


    int arr1[] = {3, 7, 2, 9};
    int arr2[] = {5, 5, 5, 5, 5, 5};
    int arr3[] = {-2, 4, -1, 6, 5};
    printf("----- UABMaxMinDiff(arr) ----- \n");
    printf("  Inputs                       Outputs \n");
    printf("arr = [ 3, 7, 2, 9]             %d\n", UABMaxMinDiff(arr1));
    printf("arr = [ 5, 5, 5, 5, 5, 5]       %d\n", UABMaxMinDiff(arr2));
    printf("arr = [ -2, 4, -1, 6, 5]        %d\n", UABMaxMinDiff(arr3));

    int arr4[] = {1, 2, 3, 4};
    int arr5[] = {2, 4, 6};
    int arr6[] = {1, 3, 5};
    replaceEvenWithZero(arr4);
    replaceEvenWithZero(arr5);
    replaceEvenWithZero(arr6);
    printf("----- replaceEvenWithZero(arr) ----- \n");
    printf("  Inputs                       Outputs \n");
    printf("arr = [ 1, 2, 3, 4]            [");
        for (int i = 0; i < 4; i++)
    {
        printf("%d", arr4[i]);

        if (i < 3)
        {
            printf(", ");
        }
    }
    printf("]\n");
    printf("arr = [ 2, 4, 6]               [");
        for (int i = 0; i < 3; i++)
    {
        printf("%d", arr5[i]);

        if (i < 2)
        {
            printf(", ");
        }
    }
    printf("]\n");
    printf("arr = [ 1, 3, 5]               [");
        for (int i = 0; i < 3; i++)
    {
        printf("%d", arr6[i]);

        if (i < 2)
        {
            printf(", ");
        }
    }
    printf("]\n");

    printf("----- perfrct Square(n) ----- \n");
    printf("  Input             Outputs \n");
    printf("  n = 16            %s\n", perfectSquare(16) ? "True" : "False");
    printf("  n = 15            %s\n", perfectSquare(15) ? "True" : "False");
    printf("  n = 25            %s\n", perfectSquare(25) ? "True" : "False");
    printf("  n = 36            %s\n", perfectSquare(36) ? "True" : "False");


    printf("----- countVowels(s) ----- \n");
    printf("  Input             Outputs \n");
    printf("  s = Hello World       %d\n", countVowels("Hello World"));
    printf("  n = UAB CS            %d\n", countVowels("UAB CS"));
    printf("  n = Python            %d\n", countVowels("Python"));
    printf("  n = aeiou             %d\n", countVowels("aeiou"));
    
}




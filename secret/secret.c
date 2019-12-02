#include <windows.h>
#include <stdio.h>

int main(){
    int secret_numbers[4] = {0xffffffff, 0xabcdabcd, 0xabcdabcd, 0xffffffff};

    printf("\nSecret values:\n");
    printf("%i\n", *(secret_numbers + 0));
    printf("%i\n", *(secret_numbers + 1));
    printf("%i\n", *(secret_numbers + 2));
    printf("%i\n", *(secret_numbers + 3));


    printf("\nMemory locations:\n");
    printf("%i\n", (secret_numbers + 0));
    printf("%i\n", (secret_numbers + 1));
    printf("%i\n", (secret_numbers + 2));
    printf("%i\n", (secret_numbers + 3));

    printf("\nWaiting...\n");

    Sleep(60000);      // wait 1min 


    printf("\nSecret values:\n");
    printf("%i\n", *(secret_numbers + 0));
    printf("%i\n", *(secret_numbers + 1));
    printf("%i\n", *(secret_numbers + 2));
    printf("%i\n", *(secret_numbers + 3));


    printf("\nMemory locations:\n");
    printf("%i\n", (secret_numbers + 0));
    printf("%i\n", (secret_numbers + 1));
    printf("%i\n", (secret_numbers + 2));
    printf("%i\n", (secret_numbers + 3));

    // Do something with the secret_numbers to make sure they don't get optimized away
    if (secret_numbers[0] == secret_numbers[3]){
        puts("\nsomething");
    }
    if (secret_numbers[1] == secret_numbers[2]){
        puts("something");
    }
    return 0;
}
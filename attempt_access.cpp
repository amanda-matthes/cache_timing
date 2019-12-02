#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    int * ptr = (int *) atoi(argv[1]);  // 0x0060fe10; //

    printf("Starting at %08x aka %s\n", ptr, argv[1]);
    
    // Sleep(5000);      // wait 1min 

    printf("Memory  : Value\n");
    for (int i = 0; i < 10; i++) {
        printf("%08x: %08x\n", ptr, *ptr);
        ptr = ptr + 8;
    }  

    printf("done. no segmentation faults.\n");
}
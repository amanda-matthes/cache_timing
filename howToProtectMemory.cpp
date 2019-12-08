// compile with g++ -std=c++11 -o howToProtectMemory.exe howToProtectMemory.cpp

/*************************************
 * This is a simple example of how to use 
 * 
 * BOOL VirtualProtect(
 *   LPVOID lpAddress,
 *   SIZE_T dwSize,
 *   DWORD  flNewProtect,
 *   PDWORD lpflOldProtect
 * );
 * 
 * to change the protection of memory pages.
 * 
 * Source: https://docs.microsoft.com/en-gb/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect?redirectedfrom=MSDN
 *************************************/

#include <iomanip>      // for time(NULL)  
#include <windows.h>    // for all the VirtualProtect stuff
#include <iostream>     // for std::cout etc.

const int secret_max    = 10; 

int main(){
    printf("\n\n");
    printf("--------------------------------\n");
    printf("START\n");
    printf("--------------------------------\n");
    printf("\n\n");
    
    srand (time(NULL));

    int secret          = rand()%secret_max;  
    int not_a_secret    = rand()%secret_max;
    std::cout << "Created random secret number between 0 and " << secret_max-1 << std::endl;

    int * s_ptr = &secret;


    DWORD oldProtect;
    if(VirtualProtect(
            s_ptr,           
            4,              // 4 bytes
            PAGE_NOACCESS,  // see Memory Protection Constants 
            &oldProtect
        )){
        std::cout << "Successfully called VirtualProtect" << std::endl;
    } else{
        std::cout << "VirtualProtect failed" << std::endl;
        return 1;
    }

    // std::cout << secret << std::endl; // This line will crash the program (cannot be caught with try and catch)
    // Also note that all pages that contain the actual secret 4 bytes will be affected. In this case, that means that not_a_secret is now also protected! Be careful! You might even loose access to the pointer to the secret. This will make it impossible to revert the protection.

    std::cout << "The secret now cannot be read anymore without crashing the program." << std::endl;

    // This can be reverted by calling VirtualProtect again with PAGE_NOACCESS. However we need the pointer for that which unfortunately, in this simple example, is also in the protected memory page. 


    printf("\n\n");
    printf("--------------------------------\n");
    printf("DONE\n");
    printf("--------------------------------\n");
    return 0;

    int playground [2048];  // for some reason the program crashes without this line, even though it's after the return statement. Maybe it needs to look like we are going to need some amount of memory before VirtualProtect works. But wouldn't the compiler optimise this line away? 
}



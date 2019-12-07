// compile with g++ -std=c++11 -IC:\\boost\\include\boost-1_71\\ -o scribble.exe scribble.cpp

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
#include <string>
#include <sstream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

const int secret_max    = 10; 
const int memory_size   = 40; // bytes

void call_attempt_access(int * ptr, bool protect_memory){
    char command[10] = ""; 
    strcat(command, "attempt_access.exe ");

    std::stringstream ss;
    ss << std::dec << (unsigned long long) ptr;  
    std::string address = ss.str();
    strcat(command, (address.c_str()));


    std::cout << "Calling attempt_access.exe";

    if (protect_memory == true){
        strcat(command, " true"); // make memory protected
        std::cout << " with protected memory." << std::endl;
    } else{
        strcat(command, " false"); 
        std::cout << std::endl;
    }

    int status = system(command);  // gives a segmentation fault (returns -1073741819) if called with for example 0

    std::cout << std::endl;

    std::cout << "Exited with status " << status << std::endl;

    if (status == -1073741819){
        std::cout << "Segmentation fault. Invalid address.\n \n";
    }
}

int main(){
    printf("\n\n");
    printf("--------------------------------\n");
    printf("START\n");
    printf("--------------------------------\n");
    printf("\n\n");

    srand (time(NULL));

    /////////////////////////////////////////////////////////
    // Create shared memory
    boost::interprocess::shared_memory_object shared_memory(
        boost::interprocess::open_or_create, 
        "shared_memory", 
        boost::interprocess::read_write
    );
    // Set size
    shared_memory.truncate(memory_size); // memory size in bytes

    printf("Created shared memory. \n \n");
    
    // Map the shared_memory in this process
    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);
    
    // Reset memory to some value
    std::memset(region.get_address(), 0xAA, region.get_size()); // seems to use a char

    printf("Mapped shared memory in main process and reset content. \n \n");


    printf("--------------------------------\n");
    printf("SHARED MEMORY\n");
    printf("Region start: %08x \n", region.get_address());
    printf("Region size:  %i   \n", region.get_size());

    printf("Memory  : Value\n");
    int *mem = static_cast<int*>(region.get_address());
    for(std::size_t i = 0; i < region.get_size()/sizeof(int); ++i){
        printf("%08x: %08x\n", mem, *mem);
        mem++;
    }
    printf("--------------------------------\n \n");


    /////////////////////////////////////////////////////////

    int * secret_ptr    = (int*) region.get_address();
    *secret_ptr         = rand()%secret_max;  
    std::cout << "Created random secret number between 0 and " << secret_max-1 << std::endl;

    std::cout << " secret_ptr: " << secret_ptr  << std::endl;
    std::cout << "*secret_ptr: " << *secret_ptr << std::endl;
    std::cout << "&secret_ptr: " << &secret_ptr << std::endl;

    std::cout << std::endl;

    std::cout << "So we have a pointer to a secret in the virtual memory. But the pointer itself is somewhere completely different. This is good because VirtualProtect affects whole pages of memory not just individual bytes, and we want to be sure that we can still use the pointer after we restrict access to the secret. Otherwise we cannot revert the memory protection." << std::endl;

    std::cout << "Now let's see how that changed our virtual memory: " << std::endl;

    std::cout << std::endl;

    call_attempt_access(secret_ptr, false);

    DWORD oldProtect;
    if(VirtualProtect(
            region.get_address(),           
            region.get_size(),              
            PAGE_NOACCESS,  // see Memory Protection Constants 
            &oldProtect
            )){
        std::cout << "Successfully set memory protection to PAGE_NOACCESS." << std::endl;
    } else{
        std::cout << "VirtualProtect failed." << std::endl;
        return 1;
    }
    std::cout << std::endl;

    std::cout << "Now we can try to read the shared memory again. This will cause a segmentation fault and crash the program. To stay in control we do this in a separate program called attempt_access.exe. If it crashes, we can pick up the pieces in this program. This is why we needed the shared memory from the boost library." << std::endl;

    std::cout << std::endl;

    call_attempt_access(secret_ptr, true);

    // std::cout << secret << std::endl; // This line will crash the program (cannot be caught with try and catch)
    // Also note that all pages that contain the actual secret 4 bytes will be affected. In this case, that means that not_a_secret is now also protected! Be careful! You might even loose access to the pointer to the secret. This will make it impossible to revert the protection.

    std::cout << "As we can see, the secret now cannot be read anymore without crashing the program. " << std::endl;

    std::cout << std::endl;




    std::cout << "Now let's make the memory public again and see if we were right." << std::endl;
    
    if(VirtualProtect(
            region.get_address(),           
            region.get_size(),              
            PAGE_READWRITE,   
            &oldProtect
        )){
        std::cout << "Successfully set memory protection to PAGE_READWRITE." << std::endl;
    } else{
        std::cout << "VirtualProtect failed." << std::endl;
        return 1;
    }

    std::cout << *secret_ptr << std::endl;


    boost::interprocess::shared_memory_object::remove("shared_memory");
    printf("Deleted shared memory. \n");


    printf("\n\n");
    printf("--------------------------------\n");
    printf("DONE\n");
    printf("--------------------------------\n");
    return 0;

    int playground [2048];  // for some reason the program crashes without this line, even though it's after the return statement. Maybe it needs to look like we are going to need some amount of memory before VirtualProtect works. But wouldn't the compiler optimise this line away? 
}



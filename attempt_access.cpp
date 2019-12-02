#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

int main(int argc, char *argv[]){

    // Use shared memory (will throw if shared_memory does not exist)
    boost::interprocess::shared_memory_object shared_memory(
        boost::interprocess::open_only, 
        "shared_memory", 
        boost::interprocess::read_write
    );
    // Map the shared_memory in this process
    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);


    printf("Let's first check out the shared memory\n");
    char *mem = static_cast<char*>(region.get_address());
    for(std::size_t i = 0; i < region.get_size(); ++i){
        printf("%08x: %08x\n", mem, *mem);
        mem++;
    }

    printf("Now let's try to access the requested location\n");

    int * ptr = (int *) atoi(argv[1]);  // 0x0060fe10; //

    printf("Starting at %08x aka %s\n", ptr, argv[1]);
    
    // Sleep(5000);      // wait 1min 

    printf("Memory  : Value\n");
    for (int i = 0; i < 10; i++) {
        printf("%08x: %08x\n", ptr, *ptr);
        ptr = ptr + 8;
    }  

    printf("done. no segmentation faults.\n");

    printf("FINISHED ATTEMPT_ACCESS \n");
    printf("----------------------------\n");
    printf("----------------------------\n");
}
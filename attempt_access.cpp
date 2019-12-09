// compile with g++ -std=c++11 -IC:\\boost\\include\boost-1_71\\ -o attempt_access.exe attempt_access.cpp

/*************************************
 * This process uses a shared_memory_object 
 * (see boost library "interprocess") called
 * "shared_memory" to access an int at a given
 * memory location and print it.
 * 
 * The process will crash if "shared_memory"
 * does not exist or if there is a segmentation
 * fault.
 * 
 *************************************/

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


    // printf("Let's first check out the shared memory\n");
    // printf("--------------------------------\n");
    // printf("SHARED MEMORY\n");
    // printf("Region start: %08x \n", region.get_address());
    // printf("Region size:  %i   \n", region.get_size());

    // printf("Memory  : Value\n");
    // int *mem = static_cast<int*>(region.get_address());
    // for(std::size_t i = 0; i < region.get_size()/sizeof(int); ++i){
    //     printf("%08x: %08x\n", mem, *mem);
    //     mem++;
    // }
    // printf("--------------------------------\n \n");
    // printf("Now let's try to access the requested location\n");

    int * ptr = (int *) atoi(argv[1]);  

    // printf("Starting at %08x aka %s\n", ptr, argv[1]);
    // printf("Memory  : Value\n");
    // for (int i = 0; i < 10; i++) {
    //     printf("%08x : %08x\n", ptr, *ptr);
    //     ptr++;
    // }  

    printf("Memory  : Value\n");
    printf("%08x : %08x\n", ptr, *ptr);

}
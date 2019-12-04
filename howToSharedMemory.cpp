// compile with g++ -std=c++11 -IC:\\boost\\include\boost-1_71\\ -o howToSharedMemory.exe howToSharedMemory.cpp

#include <stdio.h>      
#include <iostream>   
#include <windows.h>    

#include "timing.hpp"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>


int main(){
    printf("\n\n");
    printf("--------------------------------\n");
    printf("START\n");
    printf("--------------------------------\n");
    printf("\n\n");


    /////////////////////////////////////////////////////////
    // Create shared memory
    boost::interprocess::shared_memory_object shared_memory(
        boost::interprocess::open_or_create, 
        "shared_memory", 
        boost::interprocess::read_write
    );
    // Set size
    shared_memory.truncate(8); // memory size in bytes

    printf("Created shared memory. \n \n");
    
    // Map the shared_memory in this process
    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);
    
    // Reset memory to some value
    std::memset(region.get_address(), 0xBC, region.get_size()); // seems to use a char

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

    // This is a regular int with a pointer in regular memory:
    int a          = 3;
    int * a_ptr    = &a;
    printf("a             : %08x\n", a);
    printf("sizeof(a)     : %08x byte\n", sizeof(a));
    printf("sizeof(a_ptr) : %08x byte\n", sizeof(a_ptr));
    printf("a_ptr         : %08x\n", a_ptr);
    printf("*a_ptr        : %08x\n", *a_ptr);
    printf("\n\n");

    // This is an int with a pointer in the shared memory:
    int * b_ptr    = (int*) region.get_address();
    *b_ptr = 64;
    printf("*b_ptr        : %08x\n", *b_ptr);
    printf("sizeof(*b_ptr): %08x byte\n", sizeof(*b_ptr));
    printf("sizeof(b_ptr) : %08x byte\n", sizeof(b_ptr));
    printf("b_ptr         : %08x\n", b_ptr);
    printf("*b_ptr        : %08x\n", *b_ptr);

    printf("\n\n");

    /////////////////////////////////////////////////////////


    // Here you could call another process using system(other process.exe) 
    // There the shared memory can be used with:
    /**
    // Use shared memory (will throw if shared_memory does not exist)
    boost::interprocess::shared_memory_object shared_memory(
        boost::interprocess::open_only, 
        "shared_memory", 
        boost::interprocess::read_write
    );
    // Map the shared_memory in this process
    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);
    **/

    /////////////////////////////////////////////////////////

    printf("--------------------------------\n");
    printf("SHARED MEMORY\n");
    printf("Region start: %08x \n", region.get_address());
    printf("Region size:  %i   \n", region.get_size());

    printf("Memory  : Value\n");
    mem = static_cast<int*>(region.get_address());
    for(std::size_t i = 0; i < region.get_size()/sizeof(int); ++i){
        printf("%08x: %08x\n", mem, *mem);
        mem++;
    }
    printf("--------------------------------\n \n");

    /////////////////////////////////////////////////////////

    boost::interprocess::shared_memory_object::remove("shared_memory");
    printf("Deleted shared memory. \n \n");


    printf("\n\n");
    printf("--------------------------------\n");
    printf("DONE\n");
    printf("--------------------------------\n");

    return 0;
}
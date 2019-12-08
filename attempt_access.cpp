// compile with g++ -std=c++11 -IC:\\boost\\include\boost-1_71\\ -o attempt_access.exe attempt_access.cpp

/************************************
 * This will try to access 10 bytes starting at address argv[1]
 * 
 * 
 * **********************************/

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <iostream>

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

    // printf("Started attempt_access.exe\n");

    // printf("Let's first check out the shared memory\n");
    // printf("--------------------------------\n");
    // printf("SHARED MEMORY\n");
    // printf("Region start: %08x \n", region.get_address());
    // printf("Region size:  %i   \n", region.get_size());

    // printf("Memory  : Value\n");
    // int *mem = static_cast<int*>(region.get_address());
    // for(std::size_t i = 0; i < 10; ++i){
    //     printf("%08x: %08x\n", mem, *mem);
    //     mem++;
    // }
    // printf("--------------------------------\n \n");

    // printf("Now let's try to access the requested location\n");

    int * ptr = (int *) atoi(argv[1]);  // 0x0060fe10; //

    if (((std::string) argv[2]) == "true"){
        int size = atoi(argv[3]); 
        // std::cout << "Protection size: " <<  size << std::endl;
        DWORD oldProtect;
        if(VirtualProtect(
                region.get_address(),           
                size,              
                PAGE_NOACCESS,  // see Memory Protection Constants 
                &oldProtect
            )){
        } else{
            return 1;
        }
    } 

    bool verbose = (bool) atoi(argv[4]);
    
    if(verbose==true) {
        printf("Starting at %08x aka %s\n", ptr, argv[1]);
        printf("Memory   : Value\n");  
        for (int i = 0; i < 10; i++) {
            printf("%08x : %08x\n", ptr, *ptr);
            ptr++;
        }  
    } else{
        int _;
        for (int i = 0; i < 10; i++) {
            _ = *ptr;
            ptr++;
        }  
    }
}
// compile with g++ -IC:\\boost\\include\boost-1_71\\ -o main.exe main.cpp

#include <stdio.h>      
#include <iostream>   
#include <windows.h>    
#include <string>
#include <sstream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#define concat(first, second) first second  // to create cmd commands


class secret{
    private:
        int secret_number;

    public:    
    int public_number;

    secret(){
        secret_number = 0x00000042;
        public_number = 0x000000AB;
        // This wasn't working properly but I could find out that secret_number and public_number are very close together in memory (one byte apart)
        // std::cout << std::endl;
        // std::cout << "secret number            : " << secret_number << std::endl;
        // std::cout << "address of secret number : " << &secret_number << std::endl;
        // printf("%c\n", secret_number);
        // printf("%c\n", public_number);
        // printf("%08x\n", &secret_number); // these two where the only ones that worked
        // printf("%08x\n", &public_number); // these two where the only ones that worked
        // std::cout << "public number            : " << public_number << std::endl;
        // std::cout << "address of public number : " << &public_number << std::endl;
    }
};

void call_attempt_access(int * ptr){
    char command[10] = ""; 
    strcat(command, "attempt_access.exe ");

    std::stringstream ss;
    ss << ptr;  
    std::string address = ss.str();

    strcat(command, (address.c_str()));
    
    int status = system(command);  // gives a segmentation fault (returns -1073741819) if called with for example 0

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

    /////////////////////////////////////////////////////////
    // Create shared memory
    boost::interprocess::shared_memory_object shared_memory(
        boost::interprocess::open_or_create, 
        "shared_memory", 
        boost::interprocess::read_write
    );
    // Set size
    shared_memory.truncate(8);

    printf("Created shared memory. \n \n");

    // Map the shared_memory in this process
    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);

    // Reset memory to all ones
    std::memset(region.get_address(), 16, region.get_size());


    printf("Mapped shared memory in main process. \n \n");

    // TO PRINT ALL MEMORY
    char *mem = static_cast<char*>(region.get_address());
    for(std::size_t i = 0; i < region.get_size(); ++i){
        printf("%08x: %08x\n", mem, *mem);
        mem++;
    }

    printf("Region start: %08x \n", region.get_address());
    printf("Region size:  %i   \n", region.get_size());
    printf("Finished memory tests. \n \n");


    /////////////////////////////////////////////////////////
    int a          = 3;
    int * a_ptr    = &a;
    printf("a           : %08x\n", a);
    printf("sizeof(a)   : %08x byte\n", sizeof(a));
    printf("a_ptr       : %08x\n", a_ptr);
    printf("*a_ptr      : %08x\n", *a_ptr);
    printf("\n\n");

    /////////////////////////////////////////////////////////

    mem = static_cast<char*>(region.get_address());
    for(std::size_t i = 0; i < region.get_size(); ++i){
        printf("%08x: %08x\n", mem, *mem);
        mem++;
    }

    /////////////////////////////////////////////////////////
    secret s;
    // now I won't be able to read secret_number
    printf("public_number            : %08x \n", s.public_number);
    printf("address of public_number : %08x \n", &s.public_number);
    printf("I cannot access s.secret_number but I can access s.public_number and I would expect them to be close together in memory. \n");
    // printf("public number: %08x \n", s.secret_number); // not working

    printf("\n\n");
    /////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////

    // attempt to read protected memory
    call_attempt_access(a_ptr);

    
    
    // use it to calculate some memory location
    // int * ptr = (int *)0x0060fe10;



    // time the cache 

    // Delete shared memory
    boost::interprocess::shared_memory_object::remove("shared_memory");
    printf("Deleted shared memory. \n \n");

    printf("\n\n");
    printf("--------------------------------\n");
    printf("DONE\n");
    printf("--------------------------------\n");

    return 0;
}
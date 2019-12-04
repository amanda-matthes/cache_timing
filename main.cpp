// compile with g++ -std=c++11 -IC:\\boost\\include\boost-1_71\\ -o main.exe main.cpp

#include <stdio.h>      
#include <iostream>   
#include <windows.h>    
#include <string>
#include <sstream>
#include <chrono>

#include "timing.hpp"

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
        printf("---------------------\n");
        printf("Constructor called\n");

        printf("public_number : %08x \n", (*this).public_number);
        printf("&public_number: %08x \n", &(*this).public_number);
        printf("secret_number : %08x \n", (*this).secret_number);
        printf("&secret_number: %08x \n", &(*this).secret_number);
                
        printf("---------------------\n");
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
void call_attempt_access(std::string ptr){
    char command[10] = ""; 
    strcat(command, "attempt_access.exe ");

    std::stringstream ss;
    ss << ptr;  
    std::string address = ss.str();

    strcat(command, (address.c_str()));
    printf(command);
    int status = system(command);  // gives a segmentation fault (returns -1073741819) if called with for example 0

    if (status == -1073741819){
        std::cout << "Segmentation fault. Invalid address.\n \n";
    }
}

std::vector<int> garbage_bag; // collects outputs to make sure that they do not get optimized away
void save(int number){
    garbage_bag.push_back(number);
}


std::string do_stuff_with_large_arrays(){ // returns rubbish
    // do stuff to "clear" the cache
    const int size = 65536*2*2; 
    int trash [size];
    trash[0] = 13;
    for (int i = 1; i < size; i++) {
        trash[i] = (trash[i-1] * trash[i-1])%3400;
    }
    if(trash[size-1] == 123){ // this is just to make sure that the computations are not optimised away by the compiler
        return "garbage";
    } else{
        return "";
    }
}

int main(){
    printf("\n\n");
    printf("--------------------------------\n");
    printf("START\n");
    printf("--------------------------------\n");
    printf("\n\n");

    /**
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


    printf("Mapped shared memory in main process. \n \n");


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

    // printf("Finished memory tests. \n \n");


    /////////////////////////////////////////////////////////
    int a          = 3;
    int * a_ptr    = &a;
    printf("a             : %08x\n", a);
    printf("sizeof(a)     : %08x byte\n", sizeof(a));
    printf("sizeof(a_ptr) : %08x byte\n", sizeof(a_ptr));
    printf("a_ptr         : %08x\n", a_ptr);
    printf("*a_ptr        : %08x\n", *a_ptr);
    printf("\n\n");

    int * b_ptr    = (int*) region.get_address();
    *b_ptr = 64;
    printf("*b_ptr        : %08x\n", *b_ptr);
    printf("sizeof(*b_ptr): %08x byte\n", sizeof(*b_ptr));
    printf("sizeof(b_ptr) : %08x byte\n", sizeof(b_ptr));
    printf("b_ptr         : %08x\n", b_ptr);
    printf("*b_ptr        : %08x\n", *b_ptr);

    printf("\n\n");

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
    // HMMM... I DONT THINK THAT THE WHOLE PRIVATE CLASS VARIABLE THING IS WORKING
    // secret * s_ptr = (secret*) region.get_address()+10;
    // secret * data_ptr = (secret*) region.get_address()+8;
    // secret * s_ptr = new (data_ptr) secret; 
    // printf("s_ptr: %08x \n", s_ptr);

    // // now I won't be able to read secret_number
    // printf("public_number            : %08x \n", (*s_ptr).public_number);
    // printf("address of public_number : %08x \n", &((*s_ptr).public_number));
    // printf("I cannot access s.secret_number but I can access s.public_number and I would expect them to be close together in memory. \n");
    // // printf("public number: %08x \n", s.secret_number); // not working

    // printf("\n\n");
    /////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////

    // attempt to read protected memory
    // printf("Calling attempt_access.exe\n");
    // call_attempt_access(b_ptr);

    
    
    // use it to calculate some memory location
    // int * ptr = (int *)0x0060fe10;

    */

    /////////////////////////////////////////////////////////

    printf("----------------------------------------------------------------\n");
    printf("Let's play around with the cache \n \n");
    StartCounter(); EndCounter();

    unsigned char secret = 42;  // unsigned char to keep things simple

    const long int size = 65536*2;
    int playground [size];     // playground is now a pointer to the first element
    for(int i = 0; i < size; i++){playground[i] = 0;} // initialise

    // now playground[size-1] should be in the cache. Let's try to time access to it


    StartCounter();    
    // std::cout << playground[size-1] << std::endl;
    save(playground[size-1]);
    std::cout << "Cache hit : " << EndCounter() << std::endl;


    StartCounter();
    // std::cout << playground[3000] << std::endl;
    save(playground[30000]);
    std::cout << "Cache miss: " << EndCounter() << std::endl;


    // So I found out that a time of >0.0001 means cache miss 

    do_stuff_with_large_arrays();
    std::cout << "\nFlushed the cache.\n" << std::endl;

    StartCounter();    
    save(playground[size-1]);
    std::cout << "Cache miss: " << EndCounter() << std::endl;

    StartCounter();
    save(playground[30000]);
    std::cout << "Cache miss: " << EndCounter() << std::endl;

    printf("So the difference is quite obvious.\n");

    printf("----------------------------------------------------------------\n");

    printf("Now, let's try to see if we can recreate our secret value from that.\n");

    do_stuff_with_large_arrays();
    std::cout << "\nFlushed the cache.\n" << std::endl;

    save(playground[secret*512]);

    for (int i = 0; i < 45; i++){
        save(playground[secret*512]);
        StartCounter();
        save(playground[i*512]);
        float time = EndCounter();
        if (time < 0.0001){
            std::cout << i <<" Bingo! " << std::endl;
        } else{
            std::cout << i <<" Nope. " << std::endl;
        }
    }
    


    printf("----------------------------------------------------------------\n");

    // std::cout << "This took " << time_access_in_ns(playground, size-1) << "ns\n"; 
    // std::cout << "This took " << time_access_in_ns(playground, 0) << "ns\n"; 


    // printf("TESTING ACCESS\n");
    // int * iterator = playground;
    // playground[0] = 16;                      // printf("iterator : %08x \n", iterator);
    // printf("*iterator: %08x \n", *iterator);
    // iterator++;
    // printf("iterator : %08x \n", iterator);
    // printf("*iterator: %08x \n", *iterator);


    /////////////////////////////////////////////////////////

    // time the cache 

    // Delete shared memory
    /**
    boost::interprocess::shared_memory_object::remove("shared_memory");
    printf("Deleted shared memory. \n \n");
    **/
    // delete s_ptr;
    // printf("Deleted s_ptr. \n \n");

    printf("\n\n");
    printf("--------------------------------\n");
    printf("DONE\n");
    printf("--------------------------------\n");

    return 0;

}
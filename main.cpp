// compile with g++ -std=c++11 -IC:\\boost\\include\boost-1_71\\ -o main.exe main.cpp

#include <stdio.h>      
#include <iostream>   
#include <windows.h>    
#include <string>
#include <sstream>
#include <chrono>
#include <stdlib.h>

#include "timing.hpp"
#include <time.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#define concat(first, second) first second  // to create cmd commands

const long int size = 65536*2;
const int secret_max  = 10; 

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

volatile int garbage_bag[RAND_MAX]; // collects outputs to make sure that they do not get optimized away

void do_stuff_with_large_arrays(){ 
    // do stuff to "clear" the cache
    const int size = 65536*2*2; 
    volatile int trash [size];
    trash[0] = 13;
    for (volatile int i = 1; i < size; i++) {
        trash[i] = (trash[i-1] * trash[i-1])%3400 + rand();
    }
    garbage_bag[rand()] = trash[size-1];
}

void flush(int * array){
    volatile bool temp = true; //rand()%2;
    if(temp){
        garbage_bag[rand()] = array[(rand()*4)%size];
        garbage_bag[rand()] = array[(rand()*4)%size];
        garbage_bag[rand()] = array[(rand()*4)%size];
        garbage_bag[rand()] = array[(rand()*4)%size];
    } else{
        garbage_bag[rand()] = array[(rand()*4)%size];
        garbage_bag[rand()] = array[(rand()*4)%size];
        garbage_bag[rand()] = array[(rand()*4)%size];
        garbage_bag[rand()] = array[(rand()*4)%size];
    }
    do_stuff_with_large_arrays();
}

int getMinIndex(float * array, int size){
    int min = 0;
    for (int i = 0; i < size; i++) {
        if(array[i] < array[min]){
            min = i;
        }
    }
    return min;
}
int getMaxIndex(int * array, int size){
    int max = 0;
    for (int i = 0; i < size; i++) {
        if(array[i] > array[max]){
            max = i;
        }
    }
    return max;
}
int getMostCommonValue(int * array, int size){
    int counters[secret_max];

    for(int i = 0; i < secret_max; i++){counters[i] = 0;}  // initialise
    for(int i = 0; i < size; i++){counters[array[i]]++;}   // count

    return getMaxIndex(counters, secret_max);
}
int main(){
    srand (time(NULL));
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
    StartCounter(); garbage_bag[rand()] = EndCounter();

    volatile unsigned char secret = 7;  // unsigned char to keep things simple


    int playground [size];     // playground is now a pointer to the first element

    for(int i = 0; i < size; i++){playground[i] = rand();} // initialise


    volatile int index1 = size/10;
    volatile int index2 = size/3; //index1 + 8192;

    flush(playground);
    std::cout << "\nFlushed the cache.\n" << std::endl;

    StartCounter();
    garbage_bag[rand()] = playground[index1];

    std::cout << "Expected cache miss: " << EndCounter() << std::endl;

    garbage_bag[rand()] = playground[index1];
    garbage_bag[rand()] = playground[index1];


    StartCounter();
    garbage_bag[rand()] = playground[index1];

    std::cout << "Expected cache hit : " << EndCounter() << std::endl;


    flush(playground);
    std::cout << "\nFlushed the cache.\n" << std::endl;


    StartCounter();    
    garbage_bag[rand()] = playground[index2];

    std::cout << "Expected cache miss: " << EndCounter() << std::endl;

    garbage_bag[rand()] = playground[index2];
    garbage_bag[rand()] = playground[index2];


    StartCounter();
    garbage_bag[rand()] = playground[index2];

    std::cout << "Expected cache hit : " << EndCounter() << std::endl;

    printf("\nSo the difference is there but not always. (Rerun a couple of times.) \nSo the cache is hard to trick.\n");

    printf("-----------------------------------------------------\n");

    printf("Now, let's try to see if we can recreate our secret value from that.\n");

    flush(playground);
    std::cout << "\nFlushed the cache.\n" << std::endl;


    const int reps = 10;
    float times[secret_max];
    int guesses[reps];
    volatile float time;


    // for (int i = 0; i < secret_max; i++){times[i] = 0;}

    for (volatile int round = 0; round < reps; round++){
        flush(playground);
        for (volatile int i = 0; i < secret_max; i++){ 
            flush(playground);
            garbage_bag[rand()] = playground[(secret+1)*4096*2]; // playground[0] tends to always be cached

            StartCounter();
            garbage_bag[rand()] = playground[(i+1)*4096*2];
            time = EndCounter();
            times[i] = time; //times[i] += time;
        }

        if(round == 1){
            std::cout << "Example run" << std::endl;
            for (int i = 0; i < secret_max; i++){
                std::cout << "Accessing array at index [(" << i << "+1)*8192] took " << times[i] << std::endl;
            }
            std::cout << "Now do this a couple more times to make it significant. " << std::endl;
        }
        guesses[round] = getMinIndex(times, secret_max);
        std::cout << "Best guess this round: " << guesses[round] << std::endl;

    }    

    std::cout << "The secret value is: " << getMostCommonValue(guesses, reps) << std::endl;
    // for (int i = 0; i < max; i++){
    //     float time = times[i];
    //     std::cout << i << " " << time << std::endl;
    //     // if (time < 0.0001){
    //     //     std::cout << i <<" Bingo! " << time << std::endl;
    //     // } else{
    //     //     std::cout << i <<" Nope. " << time << std::endl;
    //     // }        
    // }
    


    printf("-----------------------------------------------------\n");


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

    volatile int sum = 0;
    for (auto i : garbage_bag){
        sum += i;
    }
    std::cout << sum;
    
    return 0;

}
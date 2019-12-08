// compile with g++ -std=c++11 -IC:\\boost\\include\boost-1_71\\ -o main.exe main.cpp

#include <iomanip>
#include <sstream>
#include <string> 
#include "timing.hpp"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

const long int memory_size      = 65536*2;   // size of shared memory
const long int playground_size  = 65536*2;       // size of the array we use to flush the cache
int playground [playground_size];               // the array we use to flush the cache

const int secret_max    = 10; 


volatile int garbage_bag[RAND_MAX]; // collects outputs to make sure that they do not get optimized away


void call_attempt_access(int * ptr, bool protect_memory, int protect_size, bool verbose);
void do_stuff_with_large_arrays();
void flush(int * base, int size);
int getMinIndex(float * array, int size);
int getMaxIndex(int * array, int size);
int getMostCommonValue(int * array, int size);


int main(){
    srand (time(NULL));
    for(int i = 0; i < playground_size; i++){playground[i] = rand();} // initialise
    volatile int sum = 0;
    printf("\n\n");
    printf("--------------------------------\n");
    printf("START\n");
    printf("--------------------------------\n");
    printf("\n\n");

    std::cout << "First, let's allocate some memory. I am using the boost library interprocess to make it available to multiple processes for reasons that will become clear later." << std::endl;

    // Create secret memory
    boost::interprocess::shared_memory_object shared_memory(
        boost::interprocess::open_or_create, 
        "shared_memory", 
        boost::interprocess::read_write
    );
    shared_memory.truncate(memory_size); // memory size in bytes
    boost::interprocess::mapped_region region(shared_memory, boost::interprocess::read_write);
    
    // Reset memory to some value
    std::memset(region.get_address(), 0xAA, region.get_size()); // seems to use a char

    std::cout << "Created memory." << std::endl << "Let's take a look at the first few elements." << std::endl << std::endl;

    printf("Region start: %08x \n"   , region.get_address());
    printf("Region size:  %i   \n \n", region.get_size());

    printf("Memory   : Value\n");
    int *mem = static_cast<int*>(region.get_address()); 
    for(std::size_t i = 0; i < 10; ++i){ 
        printf("%08x : %08x\n", mem, *mem);
        mem++;
    }
    call_attempt_access((int *) region.get_address(), false, 0, true);


    // printf("MEMORY REGION:\n");
    // printf("Region start: %08x \n", region.get_address());
    // printf("Region size:  %i   \n", region.get_size());
    // printf("Memory   : Value\n");
    // int *mem = static_cast<int*>(region.get_address()); 
    // for(std::size_t i = 0; i < 10; ++i){ 
    //     printf("%08x : %08x\n", mem, *mem);
    //     mem++;
    // }

    std::cout << std::endl << "Now, let's create a secret number and place it into the new memory." << std::endl;
    int * secret_ptr    = (int*) region.get_address();
    *secret_ptr         = rand()%secret_max;  
    std::cout << "Created random secret number between 0 and " << secret_max-1 << "." <<std::endl;

    std::cout << " secret_ptr: " << secret_ptr  << std::endl;
    std::cout << "*secret_ptr: " << "wouldn't you like to know." << std::endl;
    std::cout << "&secret_ptr: " << &secret_ptr << std::endl;

    std::cout << std::endl;

    std::cout << std::endl << "We can make this memory protected using VirtualProtect. This way the secret value becomes really secret." << std::endl << std::endl;

    DWORD oldProtect;
    if(VirtualProtect(
            region.get_address(),           
            sizeof(int),              
            PAGE_NOACCESS,  // see Memory Protection Constants 
            &oldProtect
            )){
        std::cout << "Successfully set memory protection to PAGE_NOACCESS." << std::endl;
    } else{
        std::cout << "VirtualProtect failed." << std::endl;
        return 1;
    }
    std::cout << std::endl;

    std::cout << "Now we can try to read the shared memory again. This will cause a segmentation fault and crash the program. To stay in control we do this in a separate program called attempt_access.exe. If it crashes, we can pick up the pieces in this program. This is why I used the shared memory from the interprocess boost library." << std::endl;

    std::cout << std::endl;

    call_attempt_access((int *) region.get_address(), true, sizeof(int), true);

    std::cout << std::endl;

    // I found that VirtualProtect protects all of our memory region up to ((int *) region.get_address() + 1024) excluding

    // This leaves everything between 
    // ((int *) region.get_address() + 1024)
    // and
    // ((int *) region.get_address() + region.get_size()/sizeof(int) - 1)

    int * base = (int *) region.get_address() + 1024;

    long int size = region.get_size()/sizeof(int) - 1024;

    std::cout << base + size << std::endl;
    std::cout << ((int *) region.get_address() + region.get_size()/sizeof(int)) << std::endl;

    // printf("%i \n", ((int *) region.get_address() + 1024));
    // printf("%i \n", ((int *) region.get_address() + region.get_size()/sizeof(int) - 1));


    std::cout << "I want to see if I can still get the secret value by timing the cache Spectre-style, I mean the exploit, not the Bond movie." << std::endl;

    printf("----------------------------------------------------------------\n");
    printf("Let's play around with the cache \n \n");
    StartCounter();
    garbage_bag[rand()] = rand();
    std::cout << EndCounter() << " ignore this line" << std::endl; // reset counter. somehow necessary and for some reason needs to be printed to work  ¯\_(ツ)_/¯

    volatile int index1 = size/10;
    volatile int index2 = size/3; 

    flush(base, size);
    std::cout << "\nFlushed the cache.\n" << std::endl;

    StartCounter();
    call_attempt_access(base+index1, true, sizeof(int), false);
    std::cout << "Expected cache miss: " << EndCounter() << std::endl;

    Sleep(500);
    call_attempt_access(base+index1, true, sizeof(int), false);
    call_attempt_access(base+index1, true, sizeof(int), false);

    StartCounter();
    call_attempt_access(base+index1, true, sizeof(int), false);
    std::cout << "Expected cache hit : " << EndCounter() << std::endl;


    flush(base, size);
    std::cout << "\nFlushed the cache.\n" << std::endl;


    StartCounter();    
    garbage_bag[rand()] = base[index2];
    std::cout << "Expected cache miss: " << EndCounter() << std::endl;

    garbage_bag[rand()] = base[index2];
    garbage_bag[rand()] = base[index2];

    StartCounter();
    garbage_bag[rand()] = base[index2];
    std::cout << "Expected cache hit : " << EndCounter() << std::endl;

    printf("\nSo the difference is there but not always. (Rerun a couple of times.) \nSo the cache is hard to trick and unpredictable.\n");


    printf("-----------------------------------------------------\n");

    printf("Now, let's try to see if we can recreate a secret value from that.\n");

    // int secret = rand()%secret_max;  
    // std::cout << "Created random secret number between 0 and " << secret_max-1 << std::endl;

    // int * s_ptr = &secret;

    
    flush(base, size);
    std::cout << "\nFlushed the cache.\n" << std::endl;

    const int reps = 20;
    float times[secret_max];
    int guesses[reps];
    volatile float time;


    for (volatile int round = 0; round < reps; round++){
        flush(base, size);
        std::cout << "round " << round << std::endl;
        for (volatile int i = 0; i < secret_max; i++){ 
            flush(base, size);
            call_attempt_access(
                base + ((*secret_ptr) * 4096 * 2), 
                true, sizeof(int), false);

            StartCounter();
            call_attempt_access(
                base + (i * 4096 * 2), 
                true, sizeof(int), false);
            // garbage_bag[rand()] = playground[(i+1)*4096*2];
            time = EndCounter();
            
            times[i] = time; //times[i] += time;
        }

        guesses[round] = getMinIndex(times, secret_max);
        if(round == 0){
            // std::cout << "Example run" << std::endl;
            // std::cout << "Before each of these accesses we call [(secret+1)*81292].\nWe will never look at that value but we can reconstruct it, \nbecause we know that it will be in the cache." << std::endl;
            // for (int i = 0; i < secret_max; i++){
            //     std::cout << "Accessing array at index [(" << i << "+1)*8192] took " << std::setw(12) << times[i] << " time units " << std::endl;
            // }
            // std::cout << "A shorter time means that it was most likely in the cache. " << std::endl;
            // std::cout << "So the best guess for the secret value this round is: " << guesses[round] << std::endl;
            // std::cout << "Now do this a couple more times to make it significant. " << std::endl;
        } else {
        std::cout << "Best guess this round: " << guesses[round] << std::endl;
        }
    }    
    int prediction = getMostCommonValue(guesses, reps);
    std::cout << "The best guess overall is : " << prediction << std::endl;

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

    std::cout << "The real secret value is  : " << *secret_ptr << std::endl;

    if(prediction == *secret_ptr){
        std::cout<<"Yay!" << std::endl;
    } else{
        std::cout<<"Damn." << std::endl;
    }
    

    printf("-----------------------------------------------------\n");



    printf("\n\n");
    printf("--------------------------------\n");
    printf("DONE\n");
    printf("--------------------------------\n");

    for (auto i : garbage_bag){
        sum += i;
    }
    std::cout << sum;
    return 0;
}

void call_attempt_access(int * ptr, bool protect_memory, int protect_size, bool verbose){
    char command[10] = ""; 
    strcat(command, "attempt_access.exe ");

    std::stringstream ss;
    ss << std::dec << (unsigned long long) ptr;  
    std::string address = ss.str();
    strcat(command, (address.c_str())); 


    if(verbose){std::cout << "Calling attempt_access.exe";}

    if (protect_memory == true){
        strcat(command, " true "); // make memory protected
        std::stringstream ss2;
        ss2 << protect_size;
        std::string size_string = ss2.str();
        strcat(command, size_string.c_str());        
        if(verbose){std::cout << " with protected memory." << std::endl;}
    } else{
        strcat(command, " false 0"); 
        if(verbose){std::cout << std::endl;}
    }
    
    (verbose==true)? strcat(command, " 1") : strcat(command, " 0");

    //std::cout << "." << command << "." << std::endl;

    int status = system(command);  // gives a segmentation fault (returns -1073741819) if called with for example 0

    if(verbose){
        std::cout << std::endl;

        std::cout << "Exited with status " << status << std::endl;

        if (status == -1073741819){
            std::cout << "Segmentation fault. Invalid address.\n \n";
        }
    }
}


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

void flush(int * base, int size){
    volatile bool temp = rand()%2;
    if(temp){
        garbage_bag[rand()] = *(base + ((rand()*4+1)%size));
        garbage_bag[rand()] = *(base + ((rand()*4+1)%size));
        garbage_bag[rand()] = *(base + ((rand()*4+1)%size));
        garbage_bag[rand()] = *(base + ((rand()*4+1)%size));
        garbage_bag[rand()] = *(base + ((rand()*4+1)%size));
        garbage_bag[rand()] = *(base + ((rand()*4+1)%size));
        garbage_bag[rand()] = *(base + ((rand()*4+1)%size));
        garbage_bag[rand()] = *(base + ((rand()*4+1)%size));
    } else{
        garbage_bag[rand()] = *(base + ((rand()*4)%size));
        garbage_bag[rand()] = *(base + ((rand()*4)%size));
        garbage_bag[rand()] = *(base + ((rand()*4)%size));
        garbage_bag[rand()] = *(base + ((rand()*4)%size));
        garbage_bag[rand()] = *(base + ((rand()*4)%size));
        garbage_bag[rand()] = *(base + ((rand()*4)%size));
        garbage_bag[rand()] = *(base + ((rand()*4)%size));
        garbage_bag[rand()] = *(base + ((rand()*4)%size));
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


// compile with g++ -std=c++11 -o main.exe main.cpp

#include <iomanip>
#include <sstream>
#include <string> 
#include "timing.hpp"

const long int size     = 65536*2;
const int secret_max    = 10; 

volatile int garbage_bag[RAND_MAX]; // collects outputs to make sure that they do not get optimized away

void do_stuff_with_large_arrays();
void flush(int * array);
int getMinIndex(float * array, int size);
int getMaxIndex(int * array, int size);
int getMostCommonValue(int * array, int size);


void call_attempt_access(int * ptr){
    char command[10] = ""; 
    strcat(command, "attempt_access.exe ");

    std::stringstream ss;
    ss << ptr;  
    std::string address = ss.str();

    strcat(command, (address.c_str()));
    
    std::cout << "Calling attempt_access.exe" << std::endl;

    int status = system(command);  // gives a segmentation fault (returns -1073741819) if called with for example 0

    std::cout << "Exited with status " << status << std::endl;

    if (status == -1073741819){
        std::cout << "Segmentation fault. Invalid address.\n \n";
    }
}

int main(){
    srand (time(NULL));
    printf("\n\n");
    printf("--------------------------------\n");
    printf("START\n");
    printf("--------------------------------\n");
    printf("\n\n");

    printf("----------------------------------------------------------------\n");
    printf("Let's play around with the cache \n \n");
    StartCounter();
    garbage_bag[rand()] = rand();
    std::cout << EndCounter() << " ignore this line" << std::endl; // reset counter. somehow necessary and for some reason needs to be printed to work  ¯\_(ツ)_/¯


    int playground [size];     // playground is now a pointer to the first element

    for(int i = 0; i < size; i++){playground[i] = rand();} // initialise


    volatile int index1 = size/10;
    volatile int index2 = size/3; 

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

    printf("\nSo the difference is there but not always. (Rerun a couple of times.) \nSo the cache is hard to trick and unpredictable.\n");

    printf("-----------------------------------------------------\n");

    printf("Now, let's try to see if we can recreate a secret value from that.\n");

    int secret = rand()%secret_max;  
    std::cout << "Created random secret number between 0 and " << secret_max-1 << std::endl;

    int * s_ptr = &secret;

    
    flush(playground);
    std::cout << "\nFlushed the cache.\n" << std::endl;

    const int reps = 20;
    float times[secret_max];
    int guesses[reps];
    volatile float time;


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

        guesses[round] = getMinIndex(times, secret_max);
        if(round == 0){
            std::cout << "Example run" << std::endl;
            std::cout << "Before each of these accesses we call [(secret+1)*81292].\nWe will never look at that value but we can reconstruct it, \nbecause we know that it will be in the cache." << std::endl;
            for (int i = 0; i < secret_max; i++){
                std::cout << "Accessing array at index [(" << i << "+1)*8192] took " << std::setw(12) << times[i] << " time units " << std::endl;
            }
            std::cout << "A shorter time means that it was most likely in the cache. " << std::endl;
            std::cout << "So the best guess for the secret value this round is: " << guesses[round] << std::endl;
            std::cout << "Now do this a couple more times to make it significant. " << std::endl;
        } else {
        std::cout << "Best guess this round: " << guesses[round] << std::endl;
        }
    }    
    int prediction = getMostCommonValue(guesses, reps);
    std::cout << "The best guess overall is : " << prediction << std::endl;
    std::cout << "The real secret value is  : " << secret << std::endl;

    if(prediction == secret){
        std::cout<<"Yay!" << std::endl;
    } else{
        std::cout<<"Damn." << std::endl;
    }
    

    printf("-----------------------------------------------------\n");



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
    volatile bool temp = rand()%2;
    if(temp){
        garbage_bag[rand()] = array[(rand()*4+1)%size];
        garbage_bag[rand()] = array[(rand()*4+1)%size];
        garbage_bag[rand()] = array[(rand()*4+1)%size];
        garbage_bag[rand()] = array[(rand()*4+1)%size];
        garbage_bag[rand()] = array[(rand()*4+1)%size];
    } else{
        garbage_bag[rand()] = array[(rand()*4)%size];
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
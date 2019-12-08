// compile with g++ -std=c++11 -o main.exe main.cpp

#include <iomanip>      // for std::setw
#include "timing.hpp"

const int secret_max    = 10; 

const long int size     = 65536*2;
int playground [size];


volatile int garbage_bag[RAND_MAX]; // collects outputs to make sure that they do not get optimized away

void do_stuff_with_large_arrays();
void flush();
int getMinIndex(float * array, int size);
int getMaxIndex(int * array, int size);
int getMostCommonValue(volatile int * array, int size);


int main(){    
    
    StartCounter();
    garbage_bag[rand()] = rand();
    std::cout << EndCounter() << " ignore this line" << std::endl; // reset counter. somehow necessary and for some reason needs to be printed to work  ¯\_(ツ)_/¯

    printf("\n\n");
    printf("--------------------------------\n");
    printf("START\n");
    printf("--------------------------------\n");
    printf("\n\n");

    srand (time(NULL));                                     // set new seed
    for(int i = 0; i < size; i++){playground[i] = rand();}  // initialise playground array

    std::cout << std::setprecision(3);  // set output float precision 



    printf("-----------------------------------------------------\n");
    std::cout << "Playing around with the cache" << std::endl << std::endl;
    flush();
       
    volatile int index = 50000;
    std::cout << "First, I created a large array of size " << size << " and\nfilled it with random integers. Let's check out an\nelement somewhere in the middle of my big array and\nsee how long it takes:" << std::endl;
    StartCounter();
    std::cout << "array[" << index << "] = " << playground[index] << std::endl;
    float time1 = EndCounter();
    std::cout << "This took " << time1 << " time units" << std::endl << std::endl;

    std::cout << "Now, we expect this part of the array to be cached.\nSo if we access it again it should be quicker:" << std::endl;

    StartCounter();
    std::cout << "array[" << index << "] = " << playground[index] << std::endl;
    float time2 = EndCounter();
    std::cout << "This took " << time2 << " time units" << std::endl;


    printf("\nSo the difference is there but not always. Try re-\nrunning this a couple of times. I found that the\nsecond access is on average 3.5x faster. We can see\nthat the effects of caching are definitely noticebale\nbut quite hard to predict and reproduce. \n \n");

    printf("-----------------------------------------------------\n");
    std::cout << "Is this good enough to infer information by memory\naccess timing?" << std::endl << std::endl;

    std::cout << "To answer this question, let's first create a simple\nsecret..." << std::endl;
    volatile int secret = rand()%secret_max;  
    std::cout << "Created random secret number between 0 and " << secret_max << "." << std::endl << std::endl;

    std::cout << "We should also make sure that nothing of interest is\ncached. To do that, we can simply do some calculations\nwith large arrays that will require lots of\ncache space..." << std::endl;
    flush();
    std::cout << "Flushed the cache.\n" << std::endl;

    const int reps = 30;
    float times[secret_max];
    volatile int guesses[reps];
    volatile float time;

    for (volatile int round = 0; round < reps; round++){
        flush();
        garbage_bag[rand()] = playground[(secret+1)*4096*2]; // playground[0] tends to always be cached
        for (volatile int i = 0; i < secret_max; i++){ 
            StartCounter();
            garbage_bag[rand()] = playground[(i+1)*4096*2];
            time = EndCounter();
            times[i] = time; //times[i] += time;
        }
        guesses[round] = getMinIndex(times, secret_max);
        
        if(round == 0){
            std::cout << "First, we call array[(secret+1)*81292] (The +1 is\nthere because array[0] is almost always cached). We\nwill never look at secret directly but we can\nreconstruct it, because we know that it will be in\nthe cache." << std::endl;
            std::cout << "In this simple example we know that secret is between\n0 and " << secret_max-1 << " so we can try all the possibilities:" << std::endl << std::endl; 
            for (int i = 0; i < secret_max; i++){
                std::cout << "Accessing array[(" << i << "+1)*8192] took " << std::setw(12) << times[i] << " time units " << std::endl;
            }
            std::cout << std::endl << "A shorter time means that it was most likely in the\ncache and we expect array[(secret+1)*81292] to be\nin the cache. " << std::endl;
            std::cout << "So the best guess for the secret value this round is: " << guesses[round] << std::endl << std::endl;

            std::cout << "Now we can repeat this a couple more times to make it\nsignificant. " << std::endl;
            std::cout << "Best guesses: " << guesses[round] << " ";
        } else {
        std::cout << guesses[round] << " "; // not printing the guess results in more optimisation and worse predictions
        }
    }    
    int prediction = getMostCommonValue(guesses, reps);
    std::cout << std::endl << std::endl;
    std::cout << "The best guess overall is : " << prediction << std::endl;
    std::cout << "The real secret value is  : " << secret << std::endl;

    if(prediction == secret){
        std::cout<<"Yay!" << std::endl << std::endl;
    } else{
        std::cout<<"Damn." << std::endl << std::endl;
    }
    
    std::cout << "I found that this works ---\% of the time." << std::endl;

    printf("-----------------------------------------------------\n");

    printf("\n\n");
    printf("--------------------------------\n");
    printf("DONE\n");
    printf("--------------------------------\n");

    volatile int sum = 0;
    for (auto i : garbage_bag){
        sum += i;
    }
    std::cout << sum << " ignore this line" << std::endl;
    
    return 0;

}


void flush(){
    // Does lots of stuff to "clear" the cache.
    const int size = 65536*2*2; 
    volatile int trash [size];
    trash[0] = 13;
    for (volatile int i = 1; i < size; i++) {
        trash[i] = (trash[i-1] * trash[i-1])%3400 + rand();
    }
    garbage_bag[rand()] = trash[size-1];
    for (volatile int i = 1; i < size; i++) {
        trash[i] = (trash[i-1] * trash[i-1])%3400 + rand();
    }
    garbage_bag[rand()] = trash[size-1];
}

int getMinIndex(float * array, int size){
    // Returns the array index with the lowest value.
    int min = 0;
    for (int i = 0; i < size; i++) {
        if(array[i] < array[min]){
            min = i;
        }
    }
    return min;
}
int getMaxIndex(int * array, int size){
    //  Returns the array index with the highest value
    int max = 0;
    for (int i = 0; i < size; i++) {
        if(array[i] > array[max]){
            max = i;
        }
    }
    return max;
}
int getMostCommonValue(volatile int * array, int size){
    // Returns the most common value in an array that only contains ints between 0 and secret_max
    int counters[secret_max];

    for(int i = 0; i < secret_max; i++){counters[i] = 0;}  // initialise
    for(int i = 0; i < size; i++){counters[array[i]]++;}   // count

    return getMaxIndex(counters, secret_max);
}
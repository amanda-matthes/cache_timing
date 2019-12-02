#include <stdio.h>      
#include <iostream>   
#include <windows.h>    
#include <string>
#include <sstream>
#include <boost/interprocess/shared_memory_object.hpp>

#define concat(first, second) first second

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
    // std::string address = "00";                                         //std::string(&s.public_number);
    // char * address = "00".c_str();
    char command[10] = ""; 
    strcat(command, "attempt_access.exe ");

    // std::string address = "00";                             // WORKING

    std::stringstream ss;
    ss << ptr;  
    std::string address = ss.str();

    strcat(command, (address.c_str()));
    
    
    //command = "attempt_access.exe 00";
    //std::cout << command << (command == "attempt_access.exe 00") << std::endl;

    int status = system(command);                                       // gives a segmentation fault (returns -1073741819) if called with for example 0

    if (status == -1073741819){
        std::cout << "Segmentation fault. Invalid address.\n";
    }
}

int main(){
    printf("\n\n");
    printf("--------------------------------\n");
    printf("START\n");
    printf("--------------------------------\n");
    printf("\n\n");

    /////////////////////////////////////////////////////////
    int a          = 3;
    int * a_ptr    = &a;
    printf("a           : %08x\n", a);
    printf("sizeof(a)   : %08x byte\n", sizeof(a));
    printf("a_ptr       : %08x\n", a_ptr);
    printf("*a_ptr      : %08x\n", *a_ptr);
    printf("\n\n");
    /////////////////////////////////////////////////////////

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


    printf("\n\n");
    printf("--------------------------------\n");
    printf("DONE\n");
    printf("--------------------------------\n");

    return 0;
}
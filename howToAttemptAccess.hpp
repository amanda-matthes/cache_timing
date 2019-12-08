// This belongs together with attempt_access.cpp and .exe
#include <string>
#include <sstream>
#include <iostream>   


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
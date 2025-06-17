#include "api/gmsh.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <dlfcn.h>


bool stdinHasData(){
    struct timespec timeout{01,01};
    fd_set fds{};
    
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    int result = pselect(STDIN_FILENO + 1, &fds, nullptr, nullptr, &timeout, nullptr);
    if(result < 0){
        std::cerr << "Error in pselect: " << std::endl;
        return false;
    }
    return result;
}

int main(){


    std::cout << gmsh::isInitialized() << std::endl;
    gmsh::initialize();
    std::cout << gmsh::isInitialized() << std::endl;

    gmsh::fltk::initialize();
    gmsh::open("./t1.geo");
    while(1){
        gmsh::fltk::wait();
        if(stdinHasData()){
            // std::cout << "stdin has data" << std::endl;
            std::string input;
            std::getline(std::cin, input);
            std::cin.clear();
            std::cout << "Ready To Take Input: " << input << std::endl;
            std::getline(std::cin, input);
            std::cout << "Input Received: " << input << std::endl;
            
        }

    }


    gmsh::finalize();

}

/*To compile, first compile shared.so using
 g++ -c -fPIC shared.cpp -o shared.o
 g++ -shared shared.o test.o -o shared.so

then compile the test program using

 g++ -c -fPIC -o test.o test.cpp
 g++ -o main test.o -lgmsh -ldl

 */

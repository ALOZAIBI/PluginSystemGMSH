#include "api/gmsh.h"
#include <iostream>
#include "shared.h"

extern "C" {
    void execute(){
        gmsh::graphics::draw();
        std::cout << ++counter << std::endl;
    }
}
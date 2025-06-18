#include "AddGeometryPlugin.h"
#include "../../api/gmsh.h"
#include <iostream>

std::string getFunctions(){
    return R"(
        {
            "functions":[
                {"name": "addRect", "returns":"void", "args":[],"event":[], "description":""},
                {"name": "addRectWH", "returns":"void", "args":["float","float"], "argNames":["Width","Height"],"event":[], "description":""},
                {"name": "addCone", "returns":"void", "args":[],"event":[], "description":""}
            ]
        }
    )";
}

void initializePlugin(){
    // std::cout << "AddBoxPlugin initialized" << std::endl;
}

void addRectWH(float width,float height){
    gmsh::model::occ::addRectangle(0, 0, 0, width, height);

    gmsh::model::occ::synchronize();

    gmsh::graphics::draw();
}

void addRect(){

    gmsh::model::occ::addRectangle(0, 0, 0, 0.1, 0.3);

    gmsh::model::occ::synchronize();

    gmsh::graphics::draw();
}

void addCone(){
    // Otherwise, create and mesh a simple geometry
    gmsh::model::occ::addCone(0, 0, 0, 1, 0, 0, 0.5, 0.1);
    gmsh::model::occ::synchronize();
    // gmsh::model::mesh::generate();
    gmsh::graphics::draw();
    // std::vector<std::string> kernels;

}
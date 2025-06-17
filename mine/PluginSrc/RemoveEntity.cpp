#include "RemoveEntity.h"
#include "../../api/gmsh.h"
#include <iostream>

void removeAll() {
    // gmsh::vectorpair tags;
    // gmsh::model::getEntities(tags);

    // //print the tags to be removed
    // std::cout << "Removing entities: ";
    // for (const auto &tag : tags) {
    //     std::cout << "{" << tag.first << ", " << tag.second << "} ";
    // }
    // gmsh::model::removeEntities(tags,true);
    // // gmsh::model::geo::remove(tags);


    gmsh::clear();
    gmsh::model::geo::synchronize();
    gmsh::graphics::draw();
}

void initializePlugin() {
    // This function is called when the plugin is loaded
    // You can perform any initialization here if needed
    // std::cout << "RemoveEntity plugin initialized" << std::endl;
}

std::string getFunctions() {
    return R"(
        {
            "functions":[
                {"name": "removeAll", "returns":"void", "args":[],"event":[], "description":"Remove all entities from the model"}
            ]
        }
    )";
}
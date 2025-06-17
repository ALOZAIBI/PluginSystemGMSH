#include "DebuggingPlugin.h"
#include "../../api/gmsh.h"
#include <iostream>
// #include <Eigen/Dense>
// #include <cmath>

// void pad(int dim, int tag) {
//     std::vector<int> up;
//     std::vector<int> down;
//     gmsh::model::getAdjacencies(dim, tag, up, down);
//     //up super elemetns
//     std::cout << "Adjacencies\nUp: ";
//     for (const auto &u : up) {
//         std::cout << u << " ";
//     }
//     //down sub elements
//     std::cout << "\nDown: ";
//     for (const auto &d : down) {
//         std::cout << d << " ";
//     }
//     std::cout << std::endl;

//     //Print the coordinates of all entities
//     std::vector<double> coord;
//     gmsh::model::getValue(dim,tag,{},coord);

//     std::cout << "Coordinates of entity (" << dim << ", " << tag << "): ";
//     for (const auto &c : coord) {
//         std::cout << c << " ";
//     }
//     std::cout << std::endl;
// }

//Remove an element from mesh
void pad(int dim, int tag) {
    std::vector<int> elementTypes;
    std::vector<std::vector<std::size_t>> elementTags;
    std::vector<std::vector<std::size_t>> nodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags);
    for(int i = 0 ;i < elementTypes.size();i++){
        switch(elementTypes[i]) {
            case 1:
                std::cout << "Line ";
            case 2:
                std::cout << "Triangle ";
                break;
            case 3:
                std::cout << "Quad ";
                break;
            default:
                std::cout << "Unknown type ";
        }
        std::cout << "|Element Tags: ";
        for(const auto &t : elementTags[i]) {
            std::cout << t << " ";
        }
        std::cout << "|Nodes: ";
        for(const auto &n : nodeTags[i]) {
            std::cout << n << " ";
        }
        std::cout << std::endl;
    }
    //Count edges before removing
    std::vector<std::size_t> edgeTags;
    std::vector<std::size_t> nadeTags;
    gmsh::model::mesh::getElementsByType(1, edgeTags, nadeTags);
    std::cout << "Edges before removing: " << edgeTags.size() << std::endl;


    //Remove element 27
    gmsh::model::mesh::removeElements(2,1,{69,70});
    // gmsh::model::mesh::removeElements(2,1,{70});

    //Count edges after removing
    edgeTags.clear();
    nadeTags.clear();
    gmsh::model::mesh::getElementsByType(1, edgeTags, nadeTags);
    std::cout << "Edges after removing: " << edgeTags.size() << std::endl;

    gmsh::graphics::draw();
    // static int boole = 1;
    // gmsh::option::setNumber("Mesh.SurfaceEdges", boole);
    // // boole = !boole;
    // std::cout << "yooo"<< boole << std::endl;
    // // gmsh::option::setNumber("Mesh.SurfaceLabels",1);
    
    // gmsh::fltk::update();
}

void clear(){
    // Clear the model
    gmsh::clear();
    // Synchronize the model to ensure all changes are applied
    gmsh::model::geo::synchronize();
    // Redraw the graphics
    gmsh::graphics::draw();
}

void dim(){
    int dim = gmsh::model::getDimension();
    std::cout << "Current model dimension: " << dim << std::endl;
}

//Remove an entity with a given dimension and tag
void rem(int dim,int tag){
    gmsh::model::setVisibility({{dim,tag}},false);
    gmsh::graphics::draw();
}

//Returns the angle ABC
//The vectors taken as aprameters hold the x and y value
float getAngle(std::vector<double> &a,std::vector<double> &b, std::vector<double> &c){
    std::pair<double,double> ab = std::make_pair(b[0] - a[0], b[1] - a[1]);
    std::pair<double,double> cb = std::make_pair(b[0] - c[0], b[1] - c[1]);


    float dot = ab.first * cb.first + ab.second * cb.second;
    float cross = ab.first * cb.second - ab.second * cb.first;
    float angle = std::atan2(cross, dot);

    return  abs(angle*180 / M_PI) ;
}

//Prints all angles with a given point as the center
void ang(int pTag){
    std::vector<int> up,down, tagsOfConnectedPoints;
    gmsh::model::getAdjacencies(0, pTag, up, down);

    //The lines that contain tag are in up
    //Foreach line, find the points that make it up (These are the points that connect to the point pTag)
    for(const int &u : up){
        std::vector<int> lUp, lDown;
        gmsh::model::getAdjacencies(1, u, lUp, lDown);
        //Add the points to tagsOfConnectedPoints excluding the point pTag
        for(const int &p : lDown){
            if(p != pTag) {
                tagsOfConnectedPoints.push_back(p);
            }
        }
    }

    //Prints the connected points
    std::cout << "Points connected to point " << pTag << ": ";
    for(const int &p : tagsOfConnectedPoints) {
        std::cout << p << " ";
    }
    std::cout << std::endl;

    std::cout <<"Angles to calculate \n";

    //If there is more than one connected point, calculate the angles
    if(tagsOfConnectedPoints.size() >1){
        //Sort the connected points
        // std::sort(tagsOfConnectedPoints.begin(), tagsOfConnectedPoints.end());
        //Gets the coordinates of point pTag
        std::vector<double> pCoord;
        gmsh::model::getValue(0, pTag, {}, pCoord);
        //For each point connected to pTag
        for(int i = 0;i<tagsOfConnectedPoints.size()-1; i++){
            //Coordinates of point i
            std::vector<double> iCoord;
            gmsh::model::getValue(0, tagsOfConnectedPoints[i], {}, iCoord);
            //For each point connected to pTag after i
            for(int j = i +1; j<tagsOfConnectedPoints.size(); j++){
                //Coordinates of point j
                std::vector<double> jCoord;
                gmsh::model::getValue(0, tagsOfConnectedPoints[j], {}, jCoord);
                //Print the angles name
                std::cout << tagsOfConnectedPoints[i] << pTag << tagsOfConnectedPoints[j] << " ";
                //Print the angle
                std::cout << getAngle(iCoord, pCoord, jCoord) << " degrees" << std::endl;
            }
        }
    }

    std::cout << std::endl;
}

std::string getFunctions() {
    return R"(
        {
            "functions":[
                {"name": "pad", "returns":"void", "args":["int","int"],"event":[], "description":"Print the adjacencies of a given entity"},
                {"name": "clear", "returns":"void", "args":[],"event":[], "description":"Clear the model and redraw the graphics"},
                {"name": "dim", "returns":"void", "args":[],"event":[], "description":"Print the current model dimension"},
                {"name": "ang", "returns":"void", "args":["int"],"event":[], "description":"Print all angles with a given point as the center"},
                {"name": "rem", "returns":"void", "args":["int","int"],"event":[], "description":"Remove an entity with a given dimension and tag"}
            ]
        }
    )";
}

void initializePlugin() {
    // This function is called when the plugin is loaded
    // You can perform any initialization here if needed
    // std::cout << "DebuggingPlugin initialized" << std::endl;
}
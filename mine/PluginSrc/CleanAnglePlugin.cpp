#define AUTHOR "Ibrahim"
#define VERSION "0.2.1"
#define DESCRIPTION "This removes angles out of range"
#define CATEGORY "Mesh Cleaning"
#define KEYWORDS "Angle, Remove"

#include "CleanAnglePlugin.h"
#include "../../api/gmsh.h"
#include "../Core.h"

#include <iostream>
#include <map>

using namespace std;

void execute(){
    //Get the min and max angle
    float minAngle = Core::getInstance()->takeFloat("Min Angle: ");
    float maxAngle = Core::getInstance()->takeFloat("Max Angle: ");
    cleanAngle(minAngle, maxAngle);
}

void initializePlugin() {
    // This function is called when the plugin is loaded
    // You can perform any initialization here if needed
    // std::cout << "CleanAnglePlugin initialized" << std::endl;
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

//Returns true if the triangle contains an invalid angle
//The parameters are the coordinates of the points of the triangle
bool triangleInvalid(float min,float max,vector<double> &a, vector<double> &b, vector<double> &c) {
    float angleA = getAngle(b, a, c);
    float angleB = getAngle(a, b, c);
    float angleC = getAngle(a, c, b);

    //Check if any of the angles is outside the range [min, max]
    return (angleA < min || angleA > max || angleB < min || angleB > max || angleC < min || angleC > max);
}

//Removes invalid elements
void removeInvalid(float min,float max,int elementType,const vector<size_t> &elTags, const vector<size_t> &nodTags){
    
    // //Get entities and print their vector pair
    // gmsh::vectorpair entities;
    // gmsh::model::getEntities(entities);
    // cout << "Entities: ";
    // for (const auto &entity : entities) {
    //     cout << "{" << entity.first << ", " << entity.second << "} ";
    // }
    // cout << endl;


    //Contains the tags for 2d entities
    vector<int> entityTags;

    //Dim and tag will be updated in getNode
    int entDim;
    int entTag;
    // vector<size_t> elementsToRemove ={49, 73, 81, 83};
    vector<size_t> elementsToRemove;
    elementsToRemove.clear();
    switch(elementType){
        //Triangle
        case 2: 

            //For every triangle check if it is valid or not
            for(int i = 0;i<elTags.size();i++){
                // coordinates of the points of the triangle will be saved here
                //(x,y,z)
                vector<vector<double>> coordinates (3);

                //Get the coordinates of the nodes of this triangle
                for(int j = 0; j<3; j++){
                    vector<double> ignore;                    
                    gmsh::model::mesh::getNode(nodTags[i*3+j], coordinates[j],ignore,entDim,entTag);
                    //If the entity is 2d and it's tag hasn't been added yet, add it
                    if(entDim == 2 && std::find(entityTags.begin(), entityTags.end(), entTag) == entityTags.end()) {
                        entityTags.push_back(entTag);
                    }
                }

                //If triangle invalid, add it to list of elements to remove
                if(triangleInvalid(min,max,coordinates[0],coordinates[1],coordinates[2])){
                    elementsToRemove.push_back(elTags[i]);
                }
            }
            break;
        //Quad
        case 3:
            break;

        default:
            break;
    }



    if (elementsToRemove.empty()) {
        std::cout << "Skipping removal: elementsToRemove is empty" << std::endl;
    } else {
            // gmsh::model::mesh::removeElements(2, entTag, elementsToRemove);
            //Go through each entity to remove
            for (const auto &tag : entityTags) {
                gmsh::model::mesh::removeElements(2, tag, elementsToRemove);
            }
    }

    gmsh::model::mesh::reclassifyNodes();


    //Print how many elements were removed
    std::cout << "Removed " << elementsToRemove.size() << " elements." << std::endl;
    //Necessary to keep edges showing. It's a bug workaround
    gmsh::option::setNumber("Mesh.SurfaceEdges",0);
    gmsh::option::setNumber("Mesh.SurfaceEdges",1);


    gmsh::graphics::draw();
}



//Hides angles that are not in the range [min, max]
//Only works for 2d models
void cleanAngle(float min, float max) {
    // //Print the dimension of the current model
    // int dim = gmsh::model::getDimension();
    // if (dim != 2) {
    //     std::cerr << "This function only works for 2D models." << std::endl;
    //     return;
    // }
    vector<int> elementTypes;
    vector<vector<size_t>> elementTags;
    vector<vector<size_t>> nodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags);
    
    for(int i = 0; i< elementTypes.size();i++){
        removeInvalid(min,max,elementTypes[i], elementTags[i], nodeTags[i]);
    }

}
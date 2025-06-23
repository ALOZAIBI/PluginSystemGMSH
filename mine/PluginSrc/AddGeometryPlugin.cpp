#define AUTHOR "The Author"
#define VERSION "0.0.0"
#define DESCRIPTION "No description provided"
#define CATEGORY "Uncategorized"
#define KEYWORDS "Keyword1,Keyword2,Keyword3"

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
    double lc = 1e-2;
    int p1 = gmsh::model::geo::addPoint(0, 0, 0, lc);
    int p2 = gmsh::model::geo::addPoint(width, 0, 0, lc);
    int p3 = gmsh::model::geo::addPoint(width, height, 0, lc);
    int p4 = gmsh::model::geo::addPoint(0, height, 0, lc);
    

    int l1 = gmsh::model::geo::addLine(p1, p2);
    int l2 = gmsh::model::geo::addLine(p3, p2);
    int l3 = gmsh::model::geo::addLine(p3, p4);
    int l4 = gmsh::model::geo::addLine(p4, p1);


    gmsh::model::geo::addCurveLoop({l4, l1, -l2, l3});

    
    gmsh::model::geo::addPlaneSurface({1});
    
    gmsh::model::geo::synchronize();
    
    gmsh::graphics::draw();
    }

void addRect(){
    double lc = 1e-2;
  int p1 = gmsh::model::geo::addPoint(0, 0, 0, lc);
  int p2 = gmsh::model::geo::addPoint(.1, 0, 0, lc);
  int p3 = gmsh::model::geo::addPoint(.1, .3, 0, lc);


  int p4 = gmsh::model::geo::addPoint(0, .3, 0, lc);


  int l1 = gmsh::model::geo::addLine(p1, p2);
  int l2 = gmsh::model::geo::addLine(p3, p2);
  int l3 = gmsh::model::geo::addLine(p3, p4);
  int l4 = gmsh::model::geo::addLine(p4, p1);


  gmsh::model::geo::addCurveLoop({l4, l1, -l2, l3});


  gmsh::model::geo::addPlaneSurface({1});


  gmsh::model::geo::synchronize();

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
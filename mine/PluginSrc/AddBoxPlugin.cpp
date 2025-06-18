#include "AddBoxPlugin.h"
#include "../../api/gmsh.h"
#include <iostream>

std::string getFunctions(){
    return R"(
        {
            "functions":[
                {"name": "addBox", "returns":"void", "args":[],"event":[], "description":""},
                {"name": "addBoxS", "returns":"void", "args":["float","float"], "argNames":["Width","Height"],"event":[], "description":""},
                {"name": "addCone", "returns":"void", "args":[],"event":[], "description":""}
            ]
        }
    )";
}

void initializePlugin(){
    // std::cout << "AddBoxPlugin initialized" << std::endl;
}

void addBoxS(float width,float height){
    // gmsh::model::add("t1");
    
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

void addBox(){
//   gmsh::model::add("t1");


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
    gmsh::model::occ::addCone(1, 0, 0, 1, 0, 0, 0.5, 0.1);
    gmsh::model::occ::synchronize();
    // gmsh::model::mesh::generate();
    gmsh::graphics::draw();
    // std::vector<std::string> kernels;

}
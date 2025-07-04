#include "gmshFacade.h"
#include "../api/gmsh.h"
#include <iostream>

void addRect(float width, float height) {

    std::cout << "Adding rectangle with width: " << width << " and height: " << height << std::endl;
    
    double lc = 1e-2;
    int p1 = gmsh::model::geo::addPoint(0, 0, 0, lc);
    int p2 = gmsh::model::geo::addPoint(width, 0, 0, lc);
    int p3 = gmsh::model::geo::addPoint(width, height, 0, lc);
    int p4 = gmsh::model::geo::addPoint(0, height, 0, lc);

    int l1 = gmsh::model::geo::addLine(p1, p2);
    int l2 = gmsh::model::geo::addLine(p2, p3);
    int l3 = gmsh::model::geo::addLine(p3, p4);
    int l4 = gmsh::model::geo::addLine(p4, p1);

    gmsh::model::geo::addCurveLoop({l1, l2, l3, l4});
    gmsh::model::geo::addPlaneSurface({1});

    gmsh::model::geo::synchronize();

    gmsh::graphics::draw();
}

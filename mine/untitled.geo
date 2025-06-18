// Gmsh project created on Fri Jun 13 16:39:33 2025
SetFactory("OpenCASCADE");
//+
Disk(1) = {-0, 0.2, 0, 0.5, 0.25};
//+
Curve Loop(2) = {4, 1, 2, 3};
//+
Plane Surface(2) = {2};

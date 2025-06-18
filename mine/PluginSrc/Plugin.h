#ifndef PLUGIN_H
#define PLUGIN_H
//So that getfunctions can return string
#include <string>

extern "C"{
    //Returns Json with plugin name, function names, arguments, return type
    std::string getFunctions();
    //Initializes Plugin variables if needed
    void initializePlugin();
}

#endif
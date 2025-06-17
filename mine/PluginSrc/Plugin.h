#ifndef PLUGIN_H
#define PLUGIN_H
//So that getfunctions can return string
#include <string>

extern "C"{
    std::string getFunctions();
    void initializePlugin();
}

#endif
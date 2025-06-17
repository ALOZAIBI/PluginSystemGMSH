#ifndef DEBUGGING_PLUGIN_H
#define DEBUGGING_PLUGIN_H

#include "Plugin.h"

extern "C"{
    //Print adjacencies of a given entity
    void pad(int dim,int tag);
    void clear();
    void dim();
    void ang(int pTag);
    void rem(int dim,int tag);
}

#endif
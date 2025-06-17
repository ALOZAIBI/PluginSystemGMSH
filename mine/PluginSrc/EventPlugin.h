#ifndef EVENTPLUGIN_H
#define EVENTPLUGIN_H
#include "Plugin.h"

int pointsAdded = 0;
extern "C"{
    //Prints text when a point is added
    void printOnAdd();
}
#endif
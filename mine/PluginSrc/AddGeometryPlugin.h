#ifndef ADDBOXPLUGIN_H
#define ADDBOXPLUGIN_H
#include "Plugin.h"

extern "C"{
    void addRect();
    void addRectWH(float width,float height);
    void addCone();
}
#endif
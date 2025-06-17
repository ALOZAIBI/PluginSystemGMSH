#ifndef ADDBOXPLUGIN_H
#define ADDBOXPLUGIN_H
#include "Plugin.h"

extern "C"{
    void addBox();
    void addBoxS(float width,float height);
    void addCone();
}
#endif
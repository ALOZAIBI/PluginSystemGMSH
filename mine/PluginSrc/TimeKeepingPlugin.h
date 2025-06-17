#ifndef TIMEKEEPING_H
#define TIMEKEEPING_H

#include "Plugin.h"
#include <ctime>


extern "C"{
    void setStartTime();
    void elapsedTime();
}

#endif
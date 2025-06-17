#include "EventPlugin.h"
#include <iostream>

//Need to use a file that extends the api, and when onAdd is called, it will trigger the pluginMnaager's event function
void printOnAdd() {
    pointsAdded++;
    std::cout << "Point added! Total points: " << pointsAdded << std::endl;
}
#define AUTHOR "Ibrahim AL OZAIBI"
#define VERSION "0.1"
#define DESCRIPTION "Tells you how long this plugin has been imported"
#define CATEGORY "Misc"
#define KEYWORDS "Time,Data"

#include "TimeKeepingPlugin.h"
#include "../PluginManager.h"
#include <iostream>

time_t startTime = 0;


void initializePlugin() {
    setStartTime();
    PluginManager::getInstance()->subscribeToEvent("onInput");
}

void execute() {
    elapsedTime();
}

void setStartTime() {
    time(&startTime);
    std::cout << "Started time" << std::endl;

}

void elapsedTime(){
    time_t endTime;
    time(&endTime);
    double elapsed = difftime(endTime, startTime);
    std::cout << "Elapsed time: " << elapsed << " seconds" << std::endl;
}
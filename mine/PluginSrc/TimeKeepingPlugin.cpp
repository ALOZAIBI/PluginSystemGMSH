#define AUTHOR "Ibrahim AL OZAIBI"
#define VERSION "0.1"
#define DESCRIPTION "Tells you how long this plugin has been imported"
#define CATEGORY "Misc"
#define KEYWORDS "Time,Data"

#include "TimeKeepingPlugin.h"
#include <iostream>

time_t startTime = 0;


void initializePlugin() {
    setStartTime();
}

std::string getFunctions() {
    return R"(
        {
            "functions":[
                {"name": "setStartTime", "returns":"void", "args":[],"event":[], "description":"Sets the start time"},
                {"name": "elapsedTime", "returns":"void", "args":[],"event":["onInput"], "description":"Prints the elapsed time since start"}
            ]
        }
    )";
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
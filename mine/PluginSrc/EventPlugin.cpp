#define AUTHOR "The Author"
#define VERSION "0.0.0"
#define DESCRIPTION "No description provided"
#define CATEGORY "Uncategorized"
#define KEYWORDS "Keyword1,Keyword2,Keyword3"

#include "EventPlugin.h"
#include <iostream>

//Need to use a file that extends the api, and when onAdd is called, it will trigger the pluginMnaager's event function
void printOnAdd() {
    pointsAdded++;
    std::cout << "Point added! Total points: " << pointsAdded << std::endl;
}
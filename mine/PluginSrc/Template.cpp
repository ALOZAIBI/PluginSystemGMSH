#define AUTHOR "The Author"
#define VERSION "0.0.0"
#define DESCRIPTION "No description provided"
#define CATEGORY "Uncategorized"
#define KEYWORDS "Keyword1,Keyword2,Keyword3"

#include "Template.h"
#include "../../api/gmsh.h"
#include "../Core.h"
#include <iostream>

//Declare variables if any, here




//Initialize the plugin, it's variable . Subscribe to event here if needed
void initializePlugin(){
    // PluginManager::getInstance()->subscribeToEvent("onInput");
}

void execute(){
    // //If you have multiple functions, prompt the user to pick
    // int choice = Core::getInstance()->promptSelection({"Function 1", "Function 2", "Function 3"}, "Choose a function:");

    // switch(choice){
    //     case 0:
    //         function1();
    //         break;
    //     case 1:
    //         function2();
    //         break;
    //     case 2:
    //         function3();
    //         break;
    //     default:
    //         std::cout << "Invalid choice." << std::endl;
    //         return;
    // }

    // //If you have only one function, just call it
    // function1();
}

void function1(){
    //If this functions uses any parameters, you can take them from the user here through Core::getInstance()->takeInt/Float("msg")
}

void function2(){

}


void function3(){

}
#include "Core.h"
#include <iostream>
#include "PluginManager.h"
#include "../api/gmsh.h"
#include <thread>
#include <mutex>
#include <fstream>

//So that we have a separate thread for handling input
struct args{
    std::shared_ptr<std::string> input;
    std::shared_ptr<std::mutex> mtx;
    //There is input ready to be handled by main thread
    std::shared_ptr<bool> ready;
};

void inputHandler(args* a){
    while(true){
        if(!*a->ready){
            std::lock_guard<std::mutex> lock(*a->mtx);
            a->input->clear(); // Clear previous input
            *a->input = Core::getInstance()->takeString("1.Plugin Function, 2.Import Plugin 3.Save State 4.Undo 5.Redo|");
            //Mark the flag as ready
            *a->ready = true;
        }
    }
}

Core* Core::instance = nullptr;

Core* Core::getInstance() {
    if (instance == nullptr) {
        instance = new Core();
    }
    return instance;
}


int Core::takeInt(std::string msg) {
    std::cout << msg;
    int num;
    std::cin >> num;
    return num;
}

std::string Core::takeString(std::string msg) {
    std::cout << msg;
    std::string str;
    std::cin >> str;
    return str;
}

float Core::takeFloat(std::string msg) {
    std::cout << msg;
    float num;
    std::cin >> num;
    return num;
}

void Core::printStack() {
    //Print the current position in the stack
    for (int i = 0; i<= undoStack.size(); i++) {
        if(i < undoStack.size())
            std::cout << undoStack[i] ;
        if(i == positionInStack){
            std::cout << "   <--Curr";
        }
        else if(i == positionInStack + 1 && positionInStack + 1 < undoStack.size()){
            std::cout << "   <--Redo";
        }
        else if(i == positionInStack - 1){
            std::cout << "   <--Undo";
        }

        std::cout << std::endl;
    }
    std::cout << std::endl;
}

//Loads the previous state
void Core::undo(){
    //Previous state is before my current position
    int undoPosition = positionInStack - 1;
    if(undoPosition < 0){
        return;
    }

    std::cout << "loading state " << undoStack[undoPosition] << endl;

    char prefix = undoStack[undoPosition];
    gmsh::open(string("undoStack/")+ prefix+".geo_unrolled");
    gmsh::merge(string("undoStack/")+ prefix+".msh");

    //Next save file will be the one after the current position
    // nextSaveFile = (prefix - 'a' + 1) % maxUndoAmount; 
    
    //Draw the model
    gmsh::graphics::draw();
    
    //Update my position to the previous state
    positionInStack = undoPosition;

    printStack();

}

void Core::redo(){
    int redoPosition = positionInStack + 1;

    if(redoPosition >= undoStack.size()){
        return;
    }


    std::cout << "loading state " << undoStack[redoPosition] << endl;

    char prefix = undoStack[redoPosition];
    gmsh::open(string("undoStack/")+ prefix+".geo_unrolled");
    gmsh::merge(string("undoStack/")+ prefix+".msh");

    //Next save file will be the one after the current position
    // nextSaveFile = (prefix - 'a' + 1) % maxUndoAmount; 

    //Draw the model
    gmsh::graphics::draw();

    //Update my position to the next state
    positionInStack = redoPosition;
    printStack();
    
}

void Core::saveState(bool updatePosition) {
    char prefix = 'a' + nextSaveFile;
    // nextSaveFile = (nextSaveFile + 1) % maxUndoAmount;
    nextSaveFile++;
    cout << "STATE SAVED AT " << prefix << endl;

    //Saves the state as files
    gmsh::write(string("undoStack/")+ prefix+".geo_unrolled");
    gmsh::write(string("undoStack/")+ prefix+".msh");

    //If this isn't the most recent position in the stack, we remove all elements after it
    if(positionInStack < (int)undoStack.size() - 1){
        //Remove all elements after the current position
        undoStack.erase(undoStack.begin() + positionInStack + 1, undoStack.end());
    }


    //Update the position in the stack if my position is not the last element
    if(updatePosition && positionInStack < maxUndoAmount -1){
        positionInStack++;
    }

    //Saves the name of the file save to the undo stack
    if(undoStack.size()< maxUndoAmount)
        undoStack.push_back(prefix);
    else{
        //If the stack is full, remove the oldest element
        undoStack.pop_front();
        undoStack.push_back(prefix);
    }

    fflush(stdout);

    printStack();

}


void Core::loop(){

    args a;
    a.input = std::make_shared<std::string>();
    a.mtx = std::make_shared<std::mutex>();
    a.ready = std::make_shared<bool>(false);

    std::thread inputThread(inputHandler, &a);

    gmsh::initialize();


    gmsh::option::setNumber("Geometry.PointLabels",1);
    gmsh::option::setNumber("Geometry.CurveLabels",1);
    // gmsh::option::setNumber("Mesh.NodeLabels",1);
    
    gmsh::option::setNumber("Mesh.SurfaceLabels",1);
    gmsh::option::setNumber("General.FltkColorScheme",1);
    gmsh::option::setNumber("General.ColorScheme",3);

    //Save initially when opening the program
    //But remain in the initial state
    saveState(false);
    
        
    while (true) {
        gmsh::fltk::wait(.01);
        if (*a.ready) {
            std::lock_guard<std::mutex> lock(*a.mtx);
            int choice = std::stoi(*a.input);
            switch(choice){
                case 1:
                    PluginManager::getInstance()->callFunction();
                    break;
                case 2:
                    PluginManager::getInstance()->importPlugin();
                    break;


                case 3:
                    saveState();
                    break;
                case 4:
                    undo();
                    break;
                case 5:
                    redo();
                    break;
                default:
                    std::cout << "Invalid option. Please try again." << std::endl;
                    continue;
            }
            //Set the flag to false since we already processed
            *a.ready = false;
        }
    }
}
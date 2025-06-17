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
    std::cout << "Undo Stack" <<" Current Position" << positionInStack << std::endl;
    int counter = 0;
    for (int i = 0; i<= undoStack.size(); i++) {
        if(i < undoStack.size())
            std::cout << undoStack[i] ;
        if(counter == positionInStack){
            std::cout << "   <--";
        }
        std::cout << std::endl;
        counter++;
    }
    std::cout << std::endl;
}

//Loads the previous state
void Core::undo(){
    if(positionInStack <= 0){
        return;
    }
    if(positionInStack > undoStack.size()){
        positionInStack = undoStack.size() - 1;
    }
    else
        positionInStack--;

    std::cout << "loading state " << undoStack[positionInStack] << endl;

    char prefix = undoStack[positionInStack];
    gmsh::open(string("undoStack/")+ prefix+".geo_unrolled");
    gmsh::merge(string("undoStack/")+ prefix+".msh");

    //Next save file will be the one after the current position
    nextSaveFile = (prefix - 'a' + 1) % maxUndoAmount; 
    
    //Draw the model
    gmsh::graphics::draw();
    
    printStack();
}

void Core::redo(){
    if(positionInStack >= maxUndoAmount -1 || (positionInStack+1 > undoStack.size())){
        return;
    }

    positionInStack++;

    std::cout << "loading state " << undoStack[positionInStack] << endl;

    char prefix = undoStack[positionInStack];
    gmsh::open(string("undoStack/")+ prefix+".geo_unrolled");
    gmsh::merge(string("undoStack/")+ prefix+".msh");

    //Next save file will be the one after the current position
    nextSaveFile = (prefix - 'a' + 1) % maxUndoAmount; 

    //Draw the model
    gmsh::graphics::draw();

    printStack();
    
}

void Core::saveState(){
    //When you save and not in the last position, remove all saves after the current position
    if(positionInStack < undoStack.size()){
        cout << "Removing all saves after position " << positionInStack << endl;
        //Remove all saves after the current position
        undoStack.erase(undoStack.begin() + positionInStack+1, undoStack.end());
        
        // positionInStack++;
    }
    
    char prefix = 'a' + nextSaveFile;
    cout << "STATE SAVED AT " << prefix << endl;
    


    gmsh::write(string("undoStack/")+ prefix+".geo_unrolled");
    gmsh::write(string("undoStack/")+ prefix+".msh");


    //Update the position in the stack
    if(positionInStack < maxUndoAmount){
        positionInStack++;
    }

        //Saves the name of the prefix to the undo stack
    if(undoStack.size()< maxUndoAmount)
        undoStack.push_back(prefix);
    else{
    //If the stack is full, remove the oldest element
        undoStack.pop_front();
        undoStack.push_back(prefix);
    }



    fflush(stdout);
    nextSaveFile = (nextSaveFile + 1) % maxUndoAmount;

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
    saveState();
    
        
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
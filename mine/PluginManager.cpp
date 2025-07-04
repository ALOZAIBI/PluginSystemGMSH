#include "PluginManager.h"
#include "TerminalColors.h"
#include "types.hpp"
#include "Core.h"
#include "dlfcn.h"
#include <iostream>
#include <filesystem>
#include <fstream>

//For cleaner output
#include<iomanip>


namespace fs = filesystem;
using namespace std;

PluginManager* PluginManager::instance = nullptr;
//The constructor
PluginManager::PluginManager(const string& pathToPlugins) : pathToPlugins(pathToPlugins) {}

PluginManager *PluginManager::getInstance() {
    if(instance ==  nullptr){
        //Uses the default value set in .h which is the path to the plugins
        instance = new PluginManager();
    }
    return instance;
}

//Prints the different parts of the output table with a width
template<typename T> void printElement(T t,const int& width){
    cout << left << setw(width-1) << setfill(' ') << t << "|";
}


//Prints the plugin and its metadata
void viewPlugin(fs::directory_entry entry,int index){
    string indexOutput = YELLOW + to_string(index) + " "+RESET;
    //Prints the index
    printElement(indexOutput, 3);
    //Prints the plugin name
    printElement(entry.path().filename().string(), 30);

    //Gets the metaData for the file
    json jMetaData;
    ifstream metaDataFile(entry.path().string() + ".meta");
    if (!metaDataFile.is_open()) {
        cerr << "Error opening metadata file for plugin: " << entry.path().string() << endl;
        return;
    }
    try{
    jMetaData = json::parse(metaDataFile);
    } catch (const json::parse_error& e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
        return;
    }
    

    //Prints the category
    printElement(jMetaData["category"].get<string>(), 20);
    //Prints the author
    printElement(jMetaData["author"].get<string>(), 20);
    //Prints the version
    printElement(jMetaData["version"].get<string>(), 8);
    //Prints the description
    printElement(jMetaData["description"].get<string>(), 80);
    //Prints the modification date
    printElement(jMetaData["modifDate"].get<string>(),25);
    cout << endl;
}

vector<string> PluginManager::viewPlugins(){
    //Prints the first row
    cout << BOLDBLACK;
    printElement("Ix", 3);
    printElement("Plugin Name", 30);
    printElement("Category", 20);
    printElement("Author", 20);
    printElement("Ver.", 8);
    printElement("Description", 80);
    printElement("Last Modif Time", 25);
    cout <<  RESET << endl;

    //To print index of plugins
    int count = 0;
    //Prints the plugins
    vector<string> filenames;
    if (fs::exists(pathToPlugins) && fs::is_directory(pathToPlugins)) {
        for (fs::directory_entry entry : fs::directory_iterator(pathToPlugins)) {
            //Check if the entry is a regular file and if it has a .so extension
            if (fs::is_regular_file(entry.status()) && entry.path().extension() == ".so") {
                count++;
                filenames.push_back(entry.path().filename().string());
                viewPlugin(entry,count);
            }
        }
    }
    return filenames;
}

void PluginManager::importPlugin(){
    vector<string> filenames = viewPlugins();
    int num = Core::getInstance()->takeInt("Select plugin to import: ");

    if (num < 1 || num > filenames.size()) {
        cout << "Invalid selection." << endl;
        return;
    }


    string pluginPath = pathToPlugins + filenames[num - 1];
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle) {
        cerr << "Cannot open library: " << dlerror() << endl;
        return;
    }
    void (*execute)() = (void(*)())dlsym(handle, "execute");
    if (!execute) {
        cerr << "Cannot find execute function: " << dlerror() << endl;
        dlclose(handle);
        return;
    }
    //Add to functions
    functionAndName func;
    func.name = filenames[num - 1];
    func.function = execute;
    functions.push_back(func);

   
    //We initialize the plugin after fetching the functions
    void (*initializePlugin)() = (void(*)())dlsym(handle, "initializePlugin");
    if (!initializePlugin) {
        cerr << "Cannot find initializePlugin function: " << dlerror() << endl;
        dlclose(handle);
        return;
    }
    //Call the initializePlugin function
    initializePlugin();
}

void PluginManager::subscribeToEvent(const string& eventName) {
    eventMap[eventName].push_back(functions.back().function);
}

//Calls all functions hooked to eventName
void PluginManager::callEventFunctions(const string& eventName){
    auto it = eventMap.find(eventName);
    if(it != eventMap.end()){
        for  (const auto& function : it->second) {
            //Call the function
            function();
        }
    }
}


//callFunction manually, pick which plugin you want to call
void PluginManager::callFunction() {
    // viewImportedPlugins();
    vector<string> pluginNames;
    for(const auto& func : functions) {
        pluginNames.push_back(func.name);
    }
    
    int chosen = Core::getInstance()->promptSelection(pluginNames,"Pick a plugin to call: ");
    
    if (chosen < 0 || chosen >= functions.size()) {
        cout << "Invalid selection." << endl;
        return;
    }

    //Call the chosen function
    functions[chosen].function();

    //Save state so we can undo back to it before calling the plugin function
    Core::getInstance()->saveState();

}

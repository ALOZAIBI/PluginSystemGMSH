#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include "types.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

struct functionAndName{
    string name;
    void(*function)();
};
class PluginManager{
    private:
        //Path to the plugins
        string pathToPlugins;
        PluginManager(const string& pathToPlugins = "./Plugins/");
        static PluginManager* instance;

        //View the plugins to be imported
        vector<string> viewPlugins();   

        vector<functionAndName> functions; //Vector of functions and their names

        //Event map to reconsider implementation maybe this will be filled by the plugin in initializePlugin through a subscribe function
        //Map ("eventName", function) for the events, will be used when events are triggered
        map<string, vector<void(*)()>> eventMap;
        
    public:
        static PluginManager* getInstance();

        //Adds a plugin to pluginMap
        void importPlugin();

        //Prints the names of the plugins inside the pluginMap
        void viewImportedPlugins();

        //call function after viewing the possible plugins
        void callFunction();

        //This will be called inside some plugin's initializePlugin function, so the last plugin in the functions vector will be the one that gets hooked to the event
        void subscribeToEvent(const string& eventName);

        

        //Calls function when event is triggered
        void callEventFunctions(const string& eventName);
};


#endif
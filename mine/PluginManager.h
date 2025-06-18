#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include "types.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;
class PluginManager{
    private:
        //Path to the plugins
        string pathToPlugins;
        PluginManager(const string& pathToPlugins = "./Plugins/");
        static PluginManager* instance;

        //View the plugins to be imported
        vector<string> viewPlugins();   

        //Map ("functionName", functionSignature) for the functions, will be used when functions are manually called
        map<string, shared_ptr<functionSignature>> functionMap;

        //Map ("eventName", listOffunctionSignatures) for the events, will be used when events are triggered
        map<string, vector<shared_ptr<functionSignature>>> eventMap;

        //Gets the functions from the plugin, then fills them in functions
        void fetchFunctions(void* handle);
        
        //To be used in get Functions
        //Gets the parameter types
        ffi_type* stringToType(const string& typeName);

        vector<ffi_type*> fetchParamTypes(const json& jFunction);
        
        vector<string> fetchParamNames(const json& jFunction);
        
    public:
        static PluginManager* getInstance();

        //Adds to list of plugins, translates JSON then adds to functions
        void importPlugin();

        //Prints the functions and their description
        void viewFunctions();

        //call function (The return type is set inside this function)
        retVariant callFunction();

        //Calls function when event is triggered
        void callEventFunctions(const string& eventName);
};


#endif
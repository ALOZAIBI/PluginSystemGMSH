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
        // return;
    }
    
    string indexOutput = YELLOW + to_string(index) + " "+RESET;
    //Prints the index
    printElement(indexOutput, 3);
    //Prints the plugin name
    printElement(entry.path().filename().string(), 30);
    //Prints the category
    printElement(jMetaData["category"].get<string>(), 20);
    //Prints the author
    printElement(jMetaData["author"].get<string>(), 20);
    //Prints the version
    printElement(jMetaData["version"].get<string>(), 8);
    //Prints the description
    printElement(jMetaData["description"].get<string>(), 80);
    cout << endl;
}

vector<string> PluginManager::viewPlugins(){
    //Prints the first row
    printElement("Ix", 3);
    printElement("Plugin Name", 30);
    printElement("Category", 20);
    printElement("Author", 20);
    printElement("Ver.", 8);
    printElement("Description", 80);
    cout << endl;

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

// vector<string> PluginManager::viewPlugins() {
//     vector<string> filenames;
//     if (fs::exists(pathToPlugins) && fs::is_directory(pathToPlugins)) {
//         for (const auto& entry : fs::directory_iterator(pathToPlugins)) {
//             if (fs::is_regular_file(entry.status())) {
//                 filenames.push_back(entry.path().filename().string());
//             }
//         }
//     }
//     int count = 0;
//     for (const string& filename : filenames) {
//         count++;
//         cout << YELLOW << count << RESET << ". " << filename << endl;
//     }
//     return filenames;
// }

ffi_type* PluginManager::stringToType(const string& typeName) {
    if (typeName == "int") {
        return &ffi_type_sint;
    } else if (typeName == "float") {
        return &ffi_type_float;
    } else if (typeName == "void") {
        return &ffi_type_void;
    }
    // Add more types as needed
    cerr << "Unknown type: " << typeName << endl;
    return nullptr; // Handle unknown types
}

vector <ffi_type*> PluginManager::fetchParamTypes(const json& jFunction) {
    //Count how many parameters there are
    int numParams = jFunction["args"].size();
    vector<ffi_type*> types(numParams);
    for (int i = 0; i < numParams; ++i) {
        const string& typeName = jFunction["args"][i];
        ffi_type* type = stringToType(typeName);
        if (type) {
            types[i] = type;
        } else {
            cerr << "Error: Unsupported parameter type '" << typeName << "'." << endl;
            types[i] = &ffi_type_void; // Default to void if unsupported
        }
    }
    return types;
}
vector <string> PluginManager::fetchParamNames(const json& jFunction) {
    //Check If the function provides parameter names
    if(jFunction.contains("argNames") == false){
        return vector<string>(); // Return an empty vector if no names are provided
    }
    int numParamNames = jFunction["argNames"].size();
    vector<string> names(numParamNames);
    for (int i = 0; i < numParamNames; ++i) {
        names[i] = jFunction["argNames"][i].get<string>();
    }
    return names;
}

void PluginManager::fetchFunctions(void* handle) {
    //Get the getFunctions function from the plugin
    string (*getFunctions)() = (string(*)())dlsym(handle, "getFunctions");
    json j;
    try {
        //Get the function signatures in json format
        j = json::parse(getFunctions());
    } catch (const std::exception& e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
        return;
    }

    //For each function
    for (const json& jFunc : j["functions"]) {
        shared_ptr<functionSignature> sign = make_shared<functionSignature>();
        sign->paramTypes = fetchParamTypes(jFunc);
        //The values will be set when the function is called
        sign->paramValues = vector<void*>(sign->paramTypes.size());

        //Get param names
        sign->paramNames = fetchParamNames(jFunc);

        sign->function = (void(*)())dlsym(handle, jFunc["name"].get<string>().c_str());
        if (!sign->function) {
            cerr << "Error loading function '" << jFunc["name"] << "': " << dlerror() << endl;
            continue; // Skip this function if it cannot be loaded
        }

        //Initialized buffer
        sign->retBuffer = 0;

        sign->returnType = jFunc["returns"].get<string>();

        //Set the CIF
        if(
            ffi_prep_cif(
                &sign->cif,
                FFI_DEFAULT_ABI,
                sign->paramTypes.size(),
                stringToType(sign->returnType),
                sign->paramTypes.data()
                // types
            ) != FFI_OK
        )
        {
            cerr << "Error preparing CIF for function: " << jFunc["name"] << endl;
            continue; // Skip this function if CIF preparation fails
        }

        //Adds function to the functionMap
        functionMap[jFunc["name"]] = sign;

        //For each event this function will hook into, add it to the eventMap
        for (const auto& event : jFunc["event"]) {
            string eventName = event.get<string>();
            eventMap[eventName].push_back(sign);
        }
    }
    
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
    fetchFunctions(handle);
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


void PluginManager::viewFunctions(){
    cout << "Functions:" << endl;
    for (const auto& [name, signature] : functionMap) {
        cout << YELLOW << name  << RESET << ", Return Type: " << signature->returnType << endl;
        //If the function has parameters we print them
        if (!signature->paramTypes.empty()) {
            cout << "parameters: ";
        }
        int count = 0;
        for (const auto& type : signature->paramTypes) {
            if (type == &ffi_type_sint) {
                cout << signature->paramNames[count] << " (int) ";
            } else if (type == &ffi_type_float) {
                cout  << signature->paramNames[count] << " (float) ";
            } else if (type == &ffi_type_void) {
                cout <<  signature->paramNames[count] << " (void) ";
            } else {
                cout <<"unknown ";
            }
            count++;
        }
        if (!signature->paramTypes.empty()) {
            cout << endl;
        }
        cout<< endl; 
    }
}


retVariant callFunctionViaSignature(shared_ptr<functionSignature> signature) {
    //Call the function with the signature
    ffi_call(
        &signature->cif,
        signature->function,
        &signature->retBuffer,
        // values
        signature->paramValues.data()
    );

    //Return as retVariant
    retVariant result;
    if(signature->returnType == "int"){
        result = (int)signature->retBuffer;
    } else if(signature->returnType == "float"){
        result = (float)signature->retBuffer;
    } else if(signature->returnType == "string"){
        result = (char*)signature->retBuffer;
    } else if(signature->returnType == "void"){
        // No return value for void functions
    } else {
        cerr << "Unsupported return type." << endl;
    }
    
    return result;
}

//Calls all functions hooked to eventName
void PluginManager::callEventFunctions(const string& eventName){
    auto it = eventMap.find(eventName);
    if(it != eventMap.end()){
        for  (const auto& signature : it->second) {
            retVariant result = callFunctionViaSignature(signature);
        }
    }
}
//callFunction manually
retVariant PluginManager::callFunction() {
    viewFunctions();
    string chosen = Core::getInstance()->takeString("Enter function name: ");
    
    int value;
    //Check if the function exists
    if (functionMap.find(chosen) == functionMap.end()) {
        cerr << "Function not found." << endl;
        return retVariant(); // Return an empty variant on error
    }
    shared_ptr<functionSignature> signature = functionMap[chosen];
    //Foreach parameter, take the value from the user
    for (size_t i = 0; i < signature->paramTypes.size(); ++i) {
        string paramName = (i < signature->paramNames.size()) ? signature->paramNames[i] : "Parameter " + std::to_string(i + 1);
        if (signature->paramTypes[i] == &ffi_type_sint) {
            value = Core::getInstance()->takeInt("Enter value for " + paramName + ": ");
            signature->paramValues[i] = new int(value); // Allocate memory for the int
        } else if (signature->paramTypes[i] == &ffi_type_float) {
            float value = Core::getInstance()->takeFloat("Enter value for " + paramName + ": ");
            signature->paramValues[i] = new float(value); // Allocate memory for the float
        } else if (signature->paramTypes[i] == &ffi_type_void) {
            signature->paramValues[i] = nullptr; // No parameters for void functions
        } else {
            cerr << "Unsupported parameter type." << endl;
            return retVariant(); // Return an empty variant on error
        }
    }

    retVariant result = callFunctionViaSignature(signature);

    //Save state so we can undo back to it before calling the plugin function
    Core::getInstance()->saveState();

    return result;
}

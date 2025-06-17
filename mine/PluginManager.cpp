#include "PluginManager.h"
#include "types.hpp"
#include "Core.h"
#include "dlfcn.h"
#include <iostream>
#include <filesystem>


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

vector<string> PluginManager::viewPlugins() {
    vector<string> filenames;
    cout << "PathTOPLUGINS:" << pathToPlugins << endl;
    if (fs::exists(pathToPlugins) && fs::is_directory(pathToPlugins)) {
        for (const auto& entry : fs::directory_iterator(pathToPlugins)) {
            if (fs::is_regular_file(entry.status())) {
                filenames.push_back(entry.path().filename().string());
            }
        }
    }
    cout << "-------------------------" << endl;
    int count = 0;
    for (const string& filename : filenames) {
        count++;
        cout << count << ". " << filename << endl;
    }
    return filenames;
}

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

void PluginManager::fetchFunctions(void* handle) {
    //Get the getFunctions function from the plugin
    string (*getFunctions)() = (string(*)())dlsym(handle, "getFunctions");
    json j;
    try {
        //Get the function signatures in json format
        j = json::parse(getFunctions());
        //print number of functions
        cout << "Number of functions: " << j["functions"].size() << endl;
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

    cout << "Selected plugin: " << filenames[num - 1] << endl;

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
        cout << name << ", Return Type: " << signature->returnType << endl;
        cout << "Parameter Types: ";
        for (const auto& type : signature->paramTypes) {
            if (type == &ffi_type_sint) {
                cout << "int ";
            } else if (type == &ffi_type_float) {
                cout << "float ";
            } else if (type == &ffi_type_void) {
                cout << "void ";
            } else {
                cout << "unknown ";
            }
        }
        cout << endl <<"---------------"<< endl;
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
        if (signature->paramTypes[i] == &ffi_type_sint) {
            value = Core::getInstance()->takeInt("Enter value for parameter " + std::to_string(i + 1) + ": ");
            signature->paramValues[i] = new int(value); // Allocate memory for the int
        } else if (signature->paramTypes[i] == &ffi_type_float) {
            float value = Core::getInstance()->takeFloat("Enter value for parameter " + std::to_string(i + 1) + ": ");
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

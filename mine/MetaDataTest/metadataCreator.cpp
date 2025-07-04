#include "dlfcn.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <chrono>
#include <ctime>

using json = nlohmann::json;
namespace fs = std::filesystem;

template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
              + system_clock::now());
    return system_clock::to_time_t(sctp);
}


//Given a .so create it's .so.meta file

int getMetaData(std::string pluginPath,char*& author, char*& version, char*& description, char*& category, char*& keywords,std::string &functionsJson) {
    std::cout << "Loading plugin from: " << pluginPath << std::endl;

    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot open library: " << dlerror() << std::endl;
        return 1;
    }

    //Fetch the creator
    author = (char*) dlsym(handle, "author");
    if (!author) {
        std::cerr << "Cannot find creator variable: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }
    // //Print creator
    // std::cout << "Author: " <<author << std::endl;
    //Fetch the version
    version = (char*) dlsym(handle, "version");
    if (!version) {
        std::cerr << "Cannot find version variable: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }
    // //Print version
    // std::cout << "Version: " << version << std::endl;
    //Fetch the description
    description = (char*) dlsym(handle, "description");
    if (!description) {
        std::cerr << "Cannot find description variable: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }
    // //Print description
    // std::cout << "Description: " << description << std::endl;
    //Fetch the category
    category = (char*) dlsym(handle, "category");
    if (!category) {
        std::cerr << "Cannot find category variable: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }
    // //Print category
    // std::cout << "Category: " << category << std::endl;
    //Fetch the keywords(separated by commas)
    keywords = (char*) dlsym(handle, "keywords");
    if (!keywords) {
        std::cerr << "Cannot find keywords variable: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }
    // //Print keywords
    // std::cout << "Keywords: " << keywords << std::endl;

    //get the functions
    std::string (*getFunctions)() = (std::string(*)())dlsym(handle, "getFunctions");
    if (!getFunctions) {
        std::cerr << "Cannot find getFunctions function: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }
    //Call the getFunctions function
    functionsJson = getFunctions();

    

    //Close the handle
    dlclose(handle);

}

std::vector<std::string> convertToStringArray(std::string input)
{
    std::replace(input.begin(), input.end(), ',', ' ');

    std::istringstream stringReader{ input };

    std::vector<std::string> result;
    std::string token;

    while (stringReader >> token)
    {
        result.push_back(token);
    }

    return result;
}

int createMetaFile(const char* author, char* version, char* description, char* category,std::vector<std::string> keywords, std::string functionsJson,char* lastWriteTime ,std::string pluginPath) {
    std::cout << "Author: " << (author ? author : "null") << std::endl;
    //Create a json object from the metadata
    json metadata;
    metadata["author"] = author;
    metadata["version"] = version;
    metadata["description"] = description;
    metadata["category"] = category;
    metadata["keywords"] = keywords;
    metadata["functions"] = json::parse(functionsJson)["functions"];
    metadata["modifDate"] = lastWriteTime; 
    //Print the actual metadata
    //Create the .meta file
    std::string metaFilePath = pluginPath + ".meta";
    std::ofstream metaFile(metaFilePath);

    if (!metaFile) {
        std::cerr << "Cannot create meta file: " << metaFilePath << std::endl;
        return 1;
    }
    metaFile << metadata.dump(4); // Pretty print with 4 spaces
    metaFile.close();
    std::cout << "Metadata created successfully at: " << metaFilePath << std::endl;
}
int main(int argc, char* argv[]) {
    
    // //Dlopen the file given by the user
    // if (argc < 2) {
    //     std::cerr << "Usage: " << argv[0] << " <path_to_plugin.so> <path_to_plugin.cpp>" << std::endl;
    //     return 1;
    // }
    // fs::path p = fs::path(argv[2]);
    // fs::file_time_type lastWriteTime = fs::last_write_time(p);
    
    // std::time_t lastWriteTimeT = to_time_t(lastWriteTime);
    // char* timeStr = std::ctime(&lastWriteTimeT);
    // size_t len = std::strlen(timeStr);
    // if (len > 0 && timeStr[len - 1] == '\n') {
    //     timeStr[len - 1] = '\0';
    // }
    // char* author, * version, * description, * category, * keywords;
    // std::string functionsJson;

    // //Call the getMetaData function
    // getMetaData(argv[1], author, version, description, category, keywords,functionsJson);    

    // std::vector<std::string> keywordsVector = convertToStringArray(keywords);

    // //Create the meta file
    // createMetaFile(author, version, description, category, keywordsVector, functionsJson, timeStr,argv[1]);
    
    

}

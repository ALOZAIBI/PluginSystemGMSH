//Default Meta Data
#ifndef AUTHOR 
#define AUTHOR "The Author"
#endif

#ifndef VERSION
#define VERSION "0.0.0"
#endif

#ifndef DESCRIPTION
#define DESCRIPTION "No description provided"
#endif

#ifndef CATEGORY
#define CATEGORY "Uncategorized"
#endif

#ifndef KEYWORDS
#define KEYWORDS ""
#endif

#ifndef PLUGIN_H
#define PLUGIN_H
//So that getfunctions can return string
#include <string>



extern "C"{
    //-----------MetaData-----------------
    char author[] = AUTHOR;
    char version[] = VERSION;
    char description[] = DESCRIPTION;
    char category[] = CATEGORY;
    char keywords[] = KEYWORDS;

    /**
     * @brief Initializes variables, sets up plugin, subscribes to events.
     * This function is called when the plugin is imported
     */
    void initializePlugin();

    void execute();

}

#endif
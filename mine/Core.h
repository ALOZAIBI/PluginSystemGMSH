#ifndef CORE_H
#define CORE_H

#include <string>
#include "types.hpp"
#include <deque>

class Core{
    private:
        static Core * instance;
        Core()= default;

        int maxUndoAmount = 7;
        //Always modulo 5, to allow for max 5 states saved.
        //0 = a, 1 = b, 2 = c, 3 = d, 4 = e
        int nextSaveFile = 0;

        int positionInStack = 0;

        //For debugging
        void printStack();
    public:
        void saveState();
        static Core * getInstance();
        void loop();
        std::deque<char> undoStack;

        int takeInt(std::string msg="");
        std::string takeString(std::string msg="");
        float takeFloat(std::string msg="");

        void undo();
        void redo();
};



#endif
#ifndef CORE_H
#define CORE_H

#include <string>
#include "types.hpp"
#include <deque>

#include <memory>

class Core {
private:
    static std::shared_ptr<Core> instance;

    // Limit of number of undo states to keep
    int maxUndoAmount = 7;

    /**
     * @brief Current position in the undo stack.
     * @note
     * - undoPosition is positionInStack - 1
     * - redoPosition is positionInStack + 1
     */
    int positionInStack = 0;

    /**
     * @brief Displays the undo stack, showing the current, undo, and redo positions.
     */
    void printStack();

public:
    //Default constructor
    Core() = default;
    /**
     * @brief Saves the current state and adds it to undoStack.
     * @param initialCall If true, set the initial save prefix to 'a'. And keep the position in the stack unchanged.
     */
    void saveState(bool initialCall = false);

    static std::shared_ptr<Core> getInstance();

    void loop();

    /**
     * @brief Holds the different saves.
     */
    std::deque<char> undoStack;

    int takeInt(std::string msg = "");
    std::string takeString(std::string msg = "");
    float takeFloat(std::string msg = "");

    void undo();
    void redo();
};



#endif
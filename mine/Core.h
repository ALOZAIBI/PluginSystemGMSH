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

    //Takes input from user
    int takeInt(const std::string& msg = "") const;
    std::string takeString(const std::string& msg = "") const;
    float takeFloat(const std::string& msg = "") const;
    //Take input variant that can work with rust

    //To test cxx
    void helloWorld() const;
    void undo();
    void redo();
};

//Non member getInstance function, so that we can use it in Rust
std::shared_ptr<Core> getCoreInstance();

//This functions returns an empty string.
//This is needed so that Rust can return strings
//CxxString is opaque in rust so it can't return it 
std::unique_ptr<std::string> get_empty_string();
#endif
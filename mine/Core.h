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

    int takeInt(const std::string& msg = "") const;
    std::string takeString(const std::string& msg = "") const;
    float takeFloat(const std::string& msg = "") const;

    /**
     * @brief Prompts the user to select an option from a list, returns the option
     * @param options A vector of strings representing the options to choose from.
     * @param msg A message to display before the options.
     * @returns The index of the selected option, or -1 if the input is invalid.
     */
    int promptSelection(const std::vector<std::string>& options, const std::string& msg = "") const;
    
    /**
     * @brief Prompts the user to select an option from a comma-separated string of options.
     * @param options A string containing options separated by commas.
     * @param msg A message to display before the options.
     * @returns The index of the selected option, or -1 if the input is invalid
     * @note This function is an alternative to promptSelection to facilitate interop with rust
     */
    int promptSelectionStringAlt(const std::string& options, const std::string& msg = "") const;    
    

    void undo();
    void redo();
};

//Non member getInstance function, so that we can use it in Rust
std::shared_ptr<Core> getCoreInstance();

#endif
#ifndef TYPES_H
#define TYPES_H

#include <ffi.h>
#include <vector>
#include <string>
#include <variant>

struct functionSignature{
    //Pointer to the function
    void(*function)();
    //The call intereface
    ffi_cif cif;
    //The parameter types
    std::vector<ffi_type*> paramTypes;
    //The parameter values
    std::vector<void*> paramValues;
    //The return buffer
    ffi_arg retBuffer;
    //The dataType of the return
    std::string returnType;
};


using retVariant = std::variant<int,std::string,float>;

#endif
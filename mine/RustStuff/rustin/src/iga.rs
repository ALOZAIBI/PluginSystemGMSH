mod core_interface;
use core_interface::ffi::{Core, getCoreInstance, addRect};
use cxx::{SharedPtr,UniquePtr, let_cxx_string, CxxString};


// use crate::core_interface;

#[unsafe(no_mangle)]
pub extern "C" fn execute(){

    cxx::let_cxx_string!(options = "Addition,Subtraction,Add Rectangle");
    cxx::let_cxx_string!(msg = "Select an operation:");

    let selection = getCoreInstance().promptSelectionStringAlt(&options, &msg);

    match selection {
        0 => addition(),
        1 => subtraction(),
        2 => addRectangle(),
        _ => println!("Invalid selection"),
    }
    
}



#[unsafe(no_mangle)]
pub extern "C" fn initializePlugin(){
    println!("RUST PLUGIN INITIALIZED");
}

#[unsafe(no_mangle)]
pub extern "C" fn addRectangle() {
    cxx::let_cxx_string!(msgW = "Enter Width:");
    cxx::let_cxx_string!(msgH = "Enter Height:");
    let width = getCoreInstance().takeFloat(&msgW);
    let height = getCoreInstance().takeFloat(&msgH);

    addRect(width, height);
}

#[unsafe(no_mangle)]
pub extern "C" fn addition() {
    cxx::let_cxx_string!(msgA = "A:");
    cxx::let_cxx_string!(msgB = "B:");
    let a = getCoreInstance().takeInt(&msgA);
    let b = getCoreInstance().takeInt(&msgB);
    println!("result is = {}",a + b);
}

#[unsafe(no_mangle)]
pub extern "C" fn subtraction() {
    cxx::let_cxx_string!(msgA = "A:");
    cxx::let_cxx_string!(msgB = "B:");
    let a = getCoreInstance().takeInt(&msgA);
    let b = getCoreInstance().takeInt(&msgB);
    println!("result is = {}",a - b);
}
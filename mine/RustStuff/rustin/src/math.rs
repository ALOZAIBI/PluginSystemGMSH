mod core_interface;
use core_interface::ffi::{Core, getCoreInstance, addRect};
use cxx::{SharedPtr,UniquePtr, let_cxx_string, CxxString};


// use crate::core_interface;

#[unsafe(no_mangle)]
pub extern "C" fn execute(){

    cxx::let_cxx_string!(options = "Addition,Subtraction");
    cxx::let_cxx_string!(msg = "Select an operation:");

    let selection = getCoreInstance().promptSelectionStringAlt(&options, &msg);

    match selection {
        0 => addition(),
        1 => subtraction(),
        _ => println!("Invalid selection"),
    }
    
}



#[unsafe(no_mangle)]
pub extern "C" fn initializePlugin(){
    println!("RUST PLUGIN INITIALIZED");
}

fn addition() {
    cxx::let_cxx_string!(msgA = "A:");
    cxx::let_cxx_string!(msgB = "B:");
    let a = getCoreInstance().takeInt(&msgA);
    let b = getCoreInstance().takeInt(&msgB);
    println!("result is = {}",a + b);
}

fn subtraction() {
    cxx::let_cxx_string!(msgA = "A:");
    cxx::let_cxx_string!(msgB = "B:");
    let a = getCoreInstance().takeInt(&msgA);
    let b = getCoreInstance().takeInt(&msgB);
    println!("result is = {}",a - b);
}
mod core_interface;
use core_interface::ffi::{Core, getCoreInstance, addRect};
use cxx::{SharedPtr,UniquePtr, let_cxx_string, CxxString};



#[unsafe(no_mangle)]
pub extern "C" fn execute(){
    // //If you have multiple functions, prompt the user to pick
    // cxx::let_cxx_string!(options = "Function 1,Function 2");
    // cxx::let_cxx_string!(msg = "Select an operation:");

    // let selection = getCoreInstance().promptSelectionStringAlt(&options, &msg);

    // match selection {
    //     0 => function1(),
    //     1 => function2(),
    //     _ => println!("Invalid selection"),
    // }
    // // If you have only one function, just call it
    // function1();
}


//Initialize the plugin, it's variable .
//For now events are not implemented in Rust
#[unsafe(no_mangle)]
pub extern "C" fn initializePlugin(){
}

fn function1() {
    //If this functions uses any parameters, you can take them from the user here through Core::getInstance()->takeInt/Float("msg")
    // cxx::let_cxx_string!(msgH = "Enter Height:");
    // let width = getCoreInstance().takeFloat(&msgH);
}

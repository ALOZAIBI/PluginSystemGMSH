mod core_interface;
use core_interface::ffi::{Core, getCoreInstance, addRect};
use cxx::{SharedPtr,UniquePtr, let_cxx_string, CxxString};


// use crate::core_interface;

#[unsafe(no_mangle)]
pub extern "C" fn execute(){
    addRectangle();
}



#[unsafe(no_mangle)]
pub extern "C" fn initializePlugin(){
    println!("RUST PLUGIN INITIALIZED");
}

fn addRectangle() {
    cxx::let_cxx_string!(msgW = "Enter Width:");
    cxx::let_cxx_string!(msgH = "Enter Height:");
    let width = getCoreInstance().takeFloat(&msgW);
    let height = getCoreInstance().takeFloat(&msgH);

    addRect(width, height);
}

use crate::core_interface;
#[unsafe(no_mangle)]
pub extern "C" fn getFunctions() -> Unique CxxString{
    //One function that hello worlds 
}

pub extern "C" fn initializePlugin(){
    println!("RUST PLUGIN INITIALIZED");
}

pub extern "C" fn helloWorld(){
    let iterations = take_int("How many times do you want to print hello world? ");
    for _ in 0..iterations {
        println!("Hello World from Rust!");
    }
}
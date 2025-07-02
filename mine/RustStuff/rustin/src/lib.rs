mod core_interface;
use core_interface::ffi::{Core, getCoreInstance, get_empty_string};
use cxx::{SharedPtr,UniquePtr, let_cxx_string, CxxString};

#[unsafe(no_mangle)]
pub extern "C" fn getFunctions() -> UniquePtr<CxxString> {
        //Since we can't return CxxString directly from Rust,
        //we will use get_empty_string function from Core.h
        //Then modify that string and return it
        let mut msg = get_empty_string();

        println!("getFunctions called from Rust");
        let json = r#"
        {
            "functions":[
                {"name": "addition", "returns":"void", "args":["int","int"], "argNames":["First num","Second num"], "event":[], "description":""}
            ]
        }
        "#;
        //Now we can modify the string
        msg.pin_mut().push_str(json);    

        return msg;

}
// use crate::core_interface;

#[unsafe(no_mangle)]
pub extern "C" fn initializePlugin(){
    println!("RUST PLUGIN INITIALIZED");
    let core: SharedPtr<Core> = getCoreInstance();
    core.helloWorld();
    
    let_cxx_string!(msg ="Enter a number from core:");
    let num = core.takeInt(&msg);
    println!("Number from core: {}", num);
}

#[unsafe(no_mangle)]
pub extern "C" fn addition(a: i32, b: i32) {
    println!("Addition called with {} and {} result is = {}", a, b, a + b);
}



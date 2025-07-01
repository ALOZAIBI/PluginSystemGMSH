#[cxx::bridge]
mod core_interface {
    
    unsafe extern "C++"{
        //Include the header file
        //No Need to give the ful path cuz in build.rs we specify the include directory
        include!("Core.h");

        type Core;
        fn get_instance() -> SharedPtr<Core>;
        fn take_int(msg: &CxxString) -> i32;
    }
}
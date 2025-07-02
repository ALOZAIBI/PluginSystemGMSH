#[cxx::bridge]
pub mod ffi {
    
    unsafe extern "C++"{
        //Include the header file
        //No Need to give the ful path cuz in build.rs we specify the include directory
        include!("Core.h");

        type Core;

        fn getCoreInstance() -> SharedPtr<Core>;
        fn helloWorld(self: &Core);
        fn takeInt(self: &Core, msg: &CxxString) -> i32;
        fn get_empty_string() -> UniquePtr<CxxString>;

        // fn take_int(msg: &CxxString) -> i32;
    }
}
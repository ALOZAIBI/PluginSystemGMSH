#[cxx::bridge]
pub mod ffi {
    
    unsafe extern "C++"{
        //Include the header file
        //No Need to give the ful path cuz in build.rs we specify the include directory
        include!("Core.h");
        include!("gmshFacade.h");

        type Core;

        fn getCoreInstance() -> SharedPtr<Core>;
        
        fn takeInt(self: &Core, msg: &CxxString) -> i32;
        fn takeFloat(self: &Core, msg: &CxxString) -> f32;

        //Options separated by commas
        fn promptSelectionStringAlt(self: &Core, options: &CxxString, msg: &CxxString) -> i32;

        fn addRect(width: f32, height: f32);

        // fn take_int(msg: &CxxString) -> i32;
    }
}
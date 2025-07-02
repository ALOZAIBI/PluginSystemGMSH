fn main(){
    cxx_build::bridge("src/core_interface.rs")
        .include("../..")
        .compile("core_interface");

    println!("cargo:rerun-if-changed=../../Core.h");
    println!("cargo:rerun-if-changed=src/core_interface.rs");
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=../../Core.cpp");

    //Link it to the Core shared library
    println!("cargo:rustc-link-lib=Core");
    println!("cargo:rustc-link-search=native=../../");

}
cmake_minimum_required(VERSION 3.20.0)
project("calc")

set(LLVM_DIR "/home/nick/Software/llvm/installation/lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)
message("Found LLVM ${LLVM_PACKAGE_VERSION}, build type ${LLVM_BUILD_TYPE}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})

# Below is to fix the linker error: undefined reference to `typeinfo for llvm::cl::GenericOptionValue'
# typeinfo errors in general hint that LLVM is built without RTTI (the default) and your client code is built with it.
# See https://discourse.llvm.org/t/undefined-reference-to-typeinfo-for-llvm-genericoptionvalue/71526/4
set(NO_RTTI "-fno-rtti")
add_definitions(${NO_RTTI})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(llvm_libs Core)

add_subdirectory("src")
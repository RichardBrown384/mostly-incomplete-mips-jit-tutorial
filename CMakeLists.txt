cmake_minimum_required(VERSION 3.16)
project(mostly-incomplete-mips-jit-tutorial)

set(CMAKE_CXX_STANDARD 20)

add_executable(tutorial
    src/CallSite.cpp
    src/CodeBuffer.cpp
    src/EmitterX64.cpp
    src/Label.cpp
    src/MIPS.cpp
    src/Mmap.cpp
    src/RecompilerState.cpp
    examples/Example1.cpp
    examples/Example2.cpp
    examples/Example3.cpp
    examples/Example4.cpp
    examples/Example5.cpp
    examples/Example6.cpp
    examples/Example7.cpp
    examples/Example8.cpp
    examples/Example9.cpp
    examples/Example10.cpp
    main.cpp
)

target_include_directories(tutorial PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
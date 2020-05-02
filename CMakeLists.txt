cmake_minimum_required(VERSION 3.16)
project(tutorial)

set(CMAKE_CXX_STANDARD 20)

add_executable(tutorial
    examples/Example1.cpp
    examples/Example2.cpp
    examples/Example3.cpp
    examples/Example4.cpp
    src/EmitterX64.cpp
    src/CodeBuffer.cpp
    src/MIPS.cpp
    src/Mmap.cpp
    main.cpp)

target_include_directories(tutorial PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
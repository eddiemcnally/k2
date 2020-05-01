#
#                               K2 Chess Engine
#
#
#
#
cmake_minimum_required(VERSION 3.0)

message("*** Setting up K2 build environment ***")


project(k2_chess_engine)

set(CMAKE_VERBOSE_MAKEFILE ON)
message("Setting CMAKE Verbose level '${CMAKE_VERBOSE_MAKEFILE}'")

set(CMAKE_C_COMPILER "/usr/bin/clang")
message("Setting CMAKE Compiler to '${CMAKE_C_COMPILER}'")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message("Setting binary output to directory to '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")
        
message("Adding project directory 'src'...")
add_subdirectory(src)
message("Adding project directory 'test'...")
add_subdirectory(test)

include(CTest)

message("*** K2 build environment - DONE ***")

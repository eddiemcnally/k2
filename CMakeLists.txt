cmake_minimum_required(VERSION 3.0)

project(k2_chess_engine)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_COMPILER "/usr/bin/clang")

# enabling this will enable engine-level asserts 
add_definitions(-DENABLE_ASSERTS)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()

#message("top ENGINE_SRCS='${ENGINE_SRCS}'")
#message("top ENGINE_INCL='${ENGINE_INCL}'")

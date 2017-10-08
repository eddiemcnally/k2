cmake_minimum_required(VERSION 3.0)

set(DEBUG_CC_FLAGS "-g -std=c11")
set(CMAKE_C_FLAGS ${DEBUG_CC_FLAGS})



# Reference the main Engine source files
include_directories(${ENGINE_INCL} ./)

set(TEST_SRCS 
	test_launcher.c 
	test_fen.c
	test_piece.c
	test_square.c
	seatest.c)

add_executable(k2_test ${TEST_SRCS} ${ENGINE_SRCS})



#message("test ENGINE_SRCS='${ENGINE_SRCS}'")
#message("test ENGINE_INCL='${ENGINE_INCL}'")


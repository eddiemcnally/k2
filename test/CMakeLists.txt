cmake_minimum_required(VERSION 3.0)

set(TEST_BINARY_NAME "test_k2")
message("Setting test binary to " ${TEST_BINARY_NAME})

set(DEBUG_CC_FLAGS 
		"-g \
		-std=c11 \
		-Wpedantic \
		-Wall \
		-Wextra \
		-fsanitize=undefined \
		-fno-omit-frame-pointer \
		-Wstrict-prototypes \
		-Wmissing-prototypes \
		-Wswitch-default \
		-Wunused-macros \
		-Wextra \
		-Wconversion \
		-Wsign-conversion \
		-Wshadow \
		-Wcast-qual \
		-Wcast-align \
		-Wmissing-include-dirs \
		-Wpointer-arith")

# set(DEBUG_CC_FLAGS "-g -std=c11")
set(CMAKE_C_FLAGS ${DEBUG_CC_FLAGS})



# Reference the main Engine source files
include_directories(${ENGINE_INCL} ./)

set(TEST_SRCS 
	test_launcher.c 
	test_fen.c
	test_piece.c
	test_square.c
	test_position.c
        test_move.c
	seatest.c)

add_executable(${TEST_BINARY_NAME} ${TEST_SRCS} ${ENGINE_SRCS})

#message("test ENGINE_SRCS='${ENGINE_SRCS}'")
#message("test ENGINE_INCL='${ENGINE_INCL}'")


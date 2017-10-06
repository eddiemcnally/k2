cmake_minimum_required(VERSION 3.0)

include_directories(${ENGINE_INCL} ./)

set(TEST_SRCS 
	test_launcher.c 
	test_fen.c
	test_piece.c
	test_square.c
	seatest.c)

add_executable(k2_test ${TEST_SRCS} ${ENGINE_SRCS})


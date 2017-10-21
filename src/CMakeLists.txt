cmake_minimum_required(VERSION 3.0)


# set up some base directories 
set(BRD_DIR 	${PROJECT_SOURCE_DIR}/src/board)
set(BB_DIR 		${PROJECT_SOURCE_DIR}/src/board/bitboard)
set(FEN_DIR 	${PROJECT_SOURCE_DIR}/src/fen)
set(POSN_DIR 	${PROJECT_SOURCE_DIR}/src/position)
set(UTILS_DIR 	${PROJECT_SOURCE_DIR}/src/utils)

message("PROJECT_SOURCE_DIR='${PROJECT_SOURCE_DIR}'")

# define all source files, except the main()
# These are shared with the test launcher.
set (SOURCES 
	${BB_DIR}/bitboard.c 
	${BB_DIR}/occupancy_mask.c 
	${BRD_DIR}/board.c 
	${BRD_DIR}/piece.c
	${BRD_DIR}/square.c
	${FEN_DIR}/fen.c
	${POSN_DIR}/position.c
	${POSN_DIR}/move.c
	)

# Define the include directories.
# Variable is shared with the test launcher
set(INCLUDES 
	${BB_DIR}
	${BRD_DIR}
	${FEN_DIR}
	${POSN_DIR}
	)

#message("main SOURCES ='${SOURCES}'")
#message("main INCLUDES ='${INCLUDES}'")


# Global variables to allow the test binary to access the main source files 
set(ENGINE_INCL ${INCLUDES} PARENT_SCOPE)
set(ENGINE_SRCS ${SOURCES} PARENT_SCOPE)


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

SET(CMAKE_C_FLAGS ${DEBUG_CC_FLAGS})

include_directories(${INCLUDES})
# note: include main.c here
add_executable(k2 main.c ${SOURCES})


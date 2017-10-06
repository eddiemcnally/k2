cmake_minimum_required(VERSION 3.0)

set(BRD_DIR 	${PROJECT_SOURCE_DIR}/src/board)
set(BB_DIR 		${PROJECT_SOURCE_DIR}/src/board/bitboard)
set(FEN_DIR 	${PROJECT_SOURCE_DIR}/src/fen)
set(POSN_DIR 	${PROJECT_SOURCE_DIR}/src/position)
set(UTILS_DIR 	${PROJECT_SOURCE_DIR}/src/utils)

set(ENGINE_SRCS 
	${BB_DIR}/bitboard.c 
	${BB_DIR}/occupancy_mask.c 
	${BRD_DIR}/board.c 
	${BRD_DIR}/piece.c
	${BRD_DIR}/square.c
	${FEN_DIR}/fen.c
	${POSN_DIR}/position.c
	PARENT_SCOPE)
set(ENGINE_INCL  
	${BB_DIR}
	${BRD_DIR}
	${FEN_DIR}
	${POSN_DIR}
	PARENT_SCOPE)

include_directories(${ENGINE_INCL})
add_executable(main_exe main.c ${ENGINE_SRCS})


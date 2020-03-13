#
#                               K2 Chess Engine
#
#       CMAKE script for building the K2 Chess Engine build environment
#
#
#
cmake_minimum_required(VERSION 3.0)

message("*** Setting up K2 Chess Engine Binary build environment ***'")

set(K2_PROJECT_SOURCE_DIR   ${PROJECT_SOURCE_DIR}/src)

message("Setting K2 Engine root directory to '${K2_PROJECT_SOURCE_DIR}'")

# set up some base directories 
set(BRD_DIR             ${K2_PROJECT_SOURCE_DIR}/board)
set(BB_DIR              ${K2_PROJECT_SOURCE_DIR}/board/bitboard)
set(OCC_GEN_DIR         ${K2_PROJECT_SOURCE_DIR}/board/bitboard/occ_mask_gen)
set(FEN_DIR             ${K2_PROJECT_SOURCE_DIR}/fen)
set(POSN_DIR            ${K2_PROJECT_SOURCE_DIR}/position)
set(MOVE_DIR            ${K2_PROJECT_SOURCE_DIR}/move)
set(UTILS_DIR           ${K2_PROJECT_SOURCE_DIR}/utils)

message("Setting up K2 Engine source files...")

set (SOURCES 
        ${BB_DIR}/bitboard.c 
        ${BB_DIR}/occupancy_mask.c
        ${OCC_GEN_DIR}/occ_mask_gen.c   
        ${BRD_DIR}/board.c 
        ${BRD_DIR}/piece.c
        ${BRD_DIR}/square.c
        ${FEN_DIR}/fen.c
        ${POSN_DIR}/position.c
        ${POSN_DIR}/hashkeys.c
        ${POSN_DIR}/move_hist.c
        ${POSN_DIR}/castle_perms.c
        ${POSN_DIR}/attack_checker.c
        ${MOVE_DIR}/move.c
        ${MOVE_DIR}/move_list.c
        ${MOVE_DIR}/move_gen.c
        ${UTILS_DIR}/utils.c
        ${UTILS_DIR}/rand.c
        )

message("Setting up K2 Engine include files...")

set(INCLUDES 
        ${BB_DIR}
        ${BRD_DIR}
        ${OCC_GEN_DIR}
        ${FEN_DIR}
        ${POSN_DIR}
        ${MOVE_DIR}
        ${UTILS_DIR}
        )

# Global variables to allow the test binary to access the main source files 
set(ENGINE_INCL ${INCLUDES} PARENT_SCOPE)
set(ENGINE_SRCS ${SOURCES} PARENT_SCOPE)

message("Setting up K2 Engine build target compiler flags...")

#//
        
set (ENGINE_COMPILER_WARNS "-std=c11 \
        -o2 \
        -fsanitize=undefined \
        -fsanitize=address \
        -fno-omit-frame-pointer \
        -fshort-enums \
        -Wpedantic \
        -Wall \
        -Werror \
        -Wextra \
        -Wformat-security \
        -Wold-style-definition \
        -Wredundant-decls \
        -Wnested-externs \
        -Wstrict-prototypes \
        -Wmissing-prototypes \
        -Wswitch-default \
        -Wunused-macros \
        -Wconversion \
        -Wsign-conversion \
        -Wshadow \
        -Wcast-qual \
        -Wcast-align \
        -Wmissing-include-dirs \
        -Wpointer-arith \
        -Wunused"
        )



# set up build target compiler flags
# set up build target compiler flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${ENGINE_COMPILER_WARNS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -o3 ${ENGINE_COMPILER_WARNS}") 


include_directories(${INCLUDES})
# note: include main.c here
add_executable(k2 main.c ${SOURCES})

message("*** K2 Chess Engine Binary build environment - DONE ***'")


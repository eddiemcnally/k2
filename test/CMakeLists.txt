#
#                               K2 Chess Engine
#
#       CMAKE script for building the Test Runner binary build environment
#
#
#
cmake_minimum_required(VERSION 3.9)

message("*** Setting up Test Runner Binary build environment ***'")


set(TEST_BINARY_NAME "k2_test_runner")
message("Setting Test Runner binary to '${TEST_BINARY_NAME}'")




set(TEST_PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/test)
message("Setting Test Runner root directory to '${TEST_PROJECT_SOURCE_DIR}'")

# set up some base directories 
set(TEST_BRD_DIR        ${TEST_PROJECT_SOURCE_DIR}/board)
set(TEST_BB_DIR         ${TEST_PROJECT_SOURCE_DIR}/board/bitboard)
set(TEST_OCC_GEN_DIR    ${TEST_PROJECT_SOURCE_DIR}/board/bitboard/occ_mask_gen)
set(TEST_FEN_DIR        ${TEST_PROJECT_SOURCE_DIR}/fen)
set(TEST_POSN_DIR       ${TEST_PROJECT_SOURCE_DIR}/position)
set(TEST_EVAL_DIR       ${TEST_PROJECT_SOURCE_DIR}/position/evaluation)
set(TEST_SEARCH_DIR     ${TEST_PROJECT_SOURCE_DIR}/position/search)
set(TEST_MOVE_DIR       ${TEST_PROJECT_SOURCE_DIR}/move)
set(TEST_UTILS_DIR      ${TEST_PROJECT_SOURCE_DIR}/utils)


message("Setting up Test Runner build target compiler flags...")




set (TEST_RUNNER_COMPILER_WARNS "-std=c11 \
        -fno-common \
        -fno-omit-frame-pointer \
        -fsanitize=undefined \
        -fshort-enums \
        -m64 \
        -o3 \
        -Waggregate-return \
        -Wall \
        -Wcast-align \
        -Wcast-qual \
        -Wcomment \
        -Wconversion \
        -Werror \
        -Wextra \
        -Wfloat-equal \
        -Winit-self \
        -Wmissing-declarations \
        -Wmissing-include-dirs \
        -Wmissing-prototypes \
        -Wnested-externs \
        -Wold-style-definition \
        -Wpedantic \
        -Wpointer-arith \
        -Wredundant-decls \
        -Wshadow \
        -Wsign-conversion \
        -Wstrict-aliasing \
        -Wstrict-overflow=5 \
        -Wstrict-prototypes \
        -Wswitch-default \
        -Wundef \
        -Wuninitialized \
        -Wunreachable-code \
        -Wunused-macros \
        -Wunused-parameter \
        -Wwrite-strings"
        )



# set up build target compiler flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${TEST_RUNNER_COMPILER_WARNS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -o3 ${TEST_RUNNER_COMPILER_WARNS}") 


# Define the include directories.
# Variable is shared with the test launcher
set(TEST_INCLUDES 
        ${TEST_BB_DIR}
        ${TEST_BRD_DIR}
        ${TEST_OCC_GEN_DIR}
        ${TEST_FEN_DIR}
        ${TEST_POSN_DIR}
        ${TEST_EVAL_DIR}
        ${TEST_MOVE_DIR}
        ${TEST_UTILS_DIR}
        ${TEST_SEARCH_DIR}       
        )


# Reference the main Engine source files
message("Setting up to reference K2 Engine include files...")
include_directories(${ENGINE_INCL} ${TEST_INCLUDES} ./)

message("Setting up to Test Runner source files...")
# setup the test runner source files
set(TEST_SRCS 
        ${TEST_PROJECT_SOURCE_DIR}/test_launcher.c
        ${TEST_BB_DIR}/test_bitboard.c 
        ${TEST_BRD_DIR}/test_board.c 
        ${TEST_BRD_DIR}/test_piece.c
        ${TEST_BRD_DIR}/test_square.c
        ${TEST_FEN_DIR}/test_fen.c
        ${TEST_POSN_DIR}/test_position.c
        ${TEST_POSN_DIR}/test_hashkeys.c
        ${TEST_POSN_DIR}/test_position_hist.c
        ${TEST_POSN_DIR}/test_castle_permissions.c
        ${TEST_POSN_DIR}/test_attack_checker.c
        ${TEST_EVAL_DIR}/test_basic_evaluator.c
        ${TEST_SEARCH_DIR}/test_transposition_table.c
        ${TEST_MOVE_DIR}/test_move.c
        ${TEST_MOVE_DIR}/test_move_list.c
        ${TEST_MOVE_DIR}/test_move_gen.c
        )

add_executable(${TEST_BINARY_NAME} ${TEST_SRCS} ${ENGINE_SRCS})

message("Setting up lib reference for cmocka...")
target_link_libraries(${TEST_BINARY_NAME} cmocka)

# CMOCKA defaults output to STDERR, so redirect for convenience
# message("Redirecting cmocka output to STDOUT...")
# set(CMOCKA_MESSAGE_OUTPUT STDOUT)

message("Enabling cmake test support...")
enable_testing()

message("Setting up ctest test runner...${TEST_BINARY_NAME}")
add_test(TestRunner ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_BINARY_NAME})


message("*** Test Runner Binary build environment - DONE ***")

#
#                               K2 Chess Engine
#
#       CMAKE script for building the Test Runner binary build environment
#
#
#
cmake_minimum_required(VERSION 3.0)

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
set(TEST_MOVE_DIR       ${TEST_PROJECT_SOURCE_DIR}/move)
set(TEST_UTILS_DIR      ${TEST_PROJECT_SOURCE_DIR}/utils)

message("Setting up Test Runner build target compiler flags...")




set (TEST_RUNNER_COMPILER_WARNS "-std=c11 \
        -o3 \
        -fsanitize=undefined \
        -fno-omit-frame-pointer \
        -fshort-enums \
        -Wpedantic \
        -Wall \
        -Waggregate-return \
        -Wextra \
        -Wfloat-equal \
        -Wundef \
        -Winit-self \
        -Wold-style-definition \
        -Wredundant-decls \
        -Wnested-externs \
        -Wstrict-prototypes \
        -Wstrict-overflow=5 \
        -Wstrict-aliasing \
        -Wmissing-declarations \
        -Wmissing-include-dirs \
        -Wno-unused-parameter \
        -Wuninitialized \
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
        -Wwrite-strings \
        -Wpointer-arith"
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
        ${TEST_MOVE_DIR}
        ${TEST_UTILS_DIR}
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
        ${TEST_POSN_DIR}/test_move_hist.c
        ${TEST_POSN_DIR}/test_castle_permissions.c
        ${TEST_POSN_DIR}/test_attack_checker.c
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

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
set(FEN_DIR             ${K2_PROJECT_SOURCE_DIR}/fen)
set(POSN_DIR            ${K2_PROJECT_SOURCE_DIR}/position)
set(EVAL_DIR            ${K2_PROJECT_SOURCE_DIR}/position/evaluation)
set(MOVE_DIR            ${K2_PROJECT_SOURCE_DIR}/move)
set(UTILS_DIR           ${K2_PROJECT_SOURCE_DIR}/utils)
set(PERFT_DIR           ${K2_PROJECT_SOURCE_DIR}/perft)
set(SEARCH_DIR          ${K2_PROJECT_SOURCE_DIR}/position/search)


message("Setting up K2 Engine source files...")

set (SOURCES 
        ${BB_DIR}/bitboard.c 
        ${BB_DIR}/occupancy_mask.c
        ${BRD_DIR}/board.c 
        ${BRD_DIR}/piece.c
        ${BRD_DIR}/square.c
        ${FEN_DIR}/fen.c
        ${POSN_DIR}/position.c
        ${POSN_DIR}/hashkeys.c
        ${POSN_DIR}/position_hist.c
        ${POSN_DIR}/castle_perms.c
        ${POSN_DIR}/attack_checker.c
        ${EVAL_DIR}/basic_evaluator.c
        ${MOVE_DIR}/move.c
        ${MOVE_DIR}/move_list.c
        ${MOVE_DIR}/move_gen.c
        ${UTILS_DIR}/utils.c
        ${UTILS_DIR}/rand.c
        ${PERFT_DIR}/perft_file_reader.c
        ${PERFT_DIR}/perft.c
        ${SEARCH_DIR}/search.c
        ${SEARCH_DIR}/transposition_table.c
        ${SEARCH_DIR}/quiesence.c
        ${SEARCH_DIR}/quiesence.c 
        ${SEARCH_DIR}/alpha_beta.c
        )

message("Setting up K2 Engine include files...")

set(INCLUDES 
        ${BB_DIR}
        ${BRD_DIR}
        ${FEN_DIR}
        ${POSN_DIR}
        ${EVAL_DIR}
        ${MOVE_DIR}
        ${UTILS_DIR}
        ${PERFT_DIR}
        ${SEARCH_DIR}
        )

# Global variables to allow the test binary to access the main source files 
set(ENGINE_INCL ${INCLUDES} PARENT_SCOPE)
set(ENGINE_SRCS ${SOURCES} PARENT_SCOPE)




message("Setting up K2 Engine build target compiler flags...")

set(CC_WARNINGS "-Wall \
-Wbad-function-cast \
-Wcast-align \
-Wcast-qual \
-Wconversion \
-Wdouble-promotion \
-Werror \
-Wextra \
-Wfloat-equal \
-Wformat-nonliteral \
-Winline \
-Winvalid-pch \
-Wmissing-declarations \
-Wmissing-format-attribute \
-Wmissing-include-dirs \
-Wmissing-prototypes \
-Wnested-externs \
-Wnull-dereference \
-Wpedantic \
-Wpointer-arith \
-Wredundant-decls \
-Wshadow \
-Wsign-conversion \
-Wstrict-aliasing=2 \
-Wstrict-overflow=5 \
-Wstrict-prototypes \
-Wswitch-default \
-Wundef \
-Wunreachable-code \
-Wunused-macros \
-Wwrite-strings"
        )

set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)




set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c17 -fsanitize=undefined ${CC_WARNINGS}")


set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -std=c17 ${CC_WARNINGS}  \
-DNDEBUG \
-flto=full \
-O3 \
-m64 \
-fno-common \
-fshort-enums \
-Winline \
-march=native \
-mtune=native")

# ---- Profiling ----
# Replace the above 'release' command with the following for Profiling
#
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -std=c17 ${CC_WARNINGS} -fshort-enums -fno-common \
#-DNDEBUG \
#-pg \
#-mtune=native")




# default to ON
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${K2_PROJECT_SOURCE_DIR}/../docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen needs to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

  

include_directories(${INCLUDES})

# *** Main ****
# note: include main.c here
message("*** Setting up main binary....")
add_executable(k2 main.c ${SOURCES})

# *** Perft ***
message("*** Setting up perft binary....")
add_executable(perft perft/perft_runner.c ${SOURCES})
message("*** Setting up post-uild copy of Perft suite test file")
add_custom_command(
        TARGET perft POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
                ${CMAKE_SOURCE_DIR}/resources/perftsuite.epd
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/perftsuite.epd)

message("*** K2 Chess Engine Binary build environment - DONE ***")


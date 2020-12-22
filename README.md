# k2
This is kestrel rev 2.

Kestrel Rev 1 was a learning experience. There was a lot of code written that became obsolete as my understanding of the mechanics surrounding a chess engine evolved.

With that understanding, K2 is a fresh start, with a better understanding of what's needed.


## CMocka
Uses Cmocka (https://cmocka.org/) as a unit test framework.

To install on openSUSE:
- sudo zypper install libcmocka0 libcmocka-devel libcmocka-devel-static


To install on Debian:
- sudo apt-get install libcmocka0 libcmocka-dev

## Doxygen
If installed, running 'make' will cause Doxygen to be executed against the source code. 
The generated output is written to the bin directory.


## Clang
K2 is built with Clang, and configured to use LTO (-flto).

To install lto support (openSUSE):
- sudo zypper install llvm-gold

## CMake and CTest
k2 uses CMake (3.9 or later) and CTest.

To build using Cmake:
- cd ./k2
- mkdir _build
- cd _build
- cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=<Debug | Release>

["cmake .. -G "Unix Makefiles" will build a Debug version by default]


You can now build and run unit tests: 
- make && ctest -VV

## Build notes:
- k2 is currently set up to use clang as the compiler.
- Some Clang-specific built-in functions are also used (eg, __builtin_bitreverse64).
    - see https://clang.llvm.org/docs/LanguageExtensions.html#builtin-functions
- The build process outputs 3 binaries: "k2", "perft" and "k2_test_runner". These can be found in the _build/bin directory.
    - "k2" is currently a place-holder
    - "perft" will run the standard 6-deep perft tests (see https://www.chessprogramming.org/Perft)
        - the perft file used is under <project>/resources/perftsuite.epd 
- Requires CMake 3.9 or greater
- Enables LTO by default; On openSUSE, has a dependency on binutils-gold and llvm9-gold


# Engine Notes
- uses bitboards (uint64_t) to represent the board, square occupancy, attack squares, etc
- uses occupancy masks for identification of target squares.
- uses a pseudo-legal move generator
- uses Hyperbola Quintessence for sliding piece move generation

# Current Status
- Verified perft test suite to depth 6 (12.2B combined moves)



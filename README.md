# k2
This is kestrel rev 2.

Kestrel Rev 1 was a learning experience. There was a lot of code written that became obsolete as my understanding of the mechanics surrounding a chess engine evolved.

With that understanding, K2 is a fresh start, with a better understanding of what's needed.


## CMocka:
Uses Cmocka (https://cmocka.org/) as a unit test framework.

To install on openSUSE:
- sudo zypper install libcmocka0 libcmocka-devel libcmocka-devel-static

To install on Debian:
- sudo apt-get install libcmocka0 libcmocka-dev


## CMake and CTest
k2 uses CMake and CTest.

To build using Cmake:
- cd ./k2
- mkdir _build
- cd _build
- cmake .. -G "Unix Makefiles"

You can now build and run unit tests: 
- make && ctest -VV

## Build notes:
- k2 is currently set up to use clang as the compiler.
- However, the code uses various GCC built-in functions
- The build process outputs 2 binaries: "k2" and "k2_test_runner". These can be found in the _build/bin directory.


# Engine Notes
- uses bitboards (uint64_t) to represent the board, square occupancy, attack squares, etc
- uses occupancy masks for identification of target squares.
- uses a pseudo-legal move generator
- uses Hyperbola Quintessence for sliding piece move generation

# Current Status
- Move generation for all pieces is complete
- Initial dev has started for position management

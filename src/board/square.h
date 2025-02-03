/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without 
 *  restriction, including without limitation the rights to use, 
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following 
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma once

#include <stdbool.h>
#include <stdio.h>

#define NUM_SQUARES (64)

// clang-format off
enum square {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2, 
    a3, b3, c3, d3, e3, f3, g3, h3, 
    a4, b4, c4, d4, e4, f4, g4, h4, 
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,

    NO_SQUARE
};

enum rank { 
    RANK_1, 
    RANK_2, 
    RANK_3, 
    RANK_4, 
    RANK_5, 
    RANK_6, 
    RANK_7, 
    RANK_8
};

enum file { 
    FILE_A, 
    FILE_B, 
    FILE_C, 
    FILE_D, 
    FILE_E, 
    FILE_F, 
    FILE_G, 
    FILE_H 
};

#define NUM_RANKS   8
#define NUM_FILES   8

// clang-format on

enum rank sq_get_rank(enum square sq);
enum file sq_get_file(enum square sq);
enum square sq_gen_from_rank_file(enum rank rank, enum file file);
enum square sq_get_square_plus_1_rank(enum square from_sq);
enum square sq_get_square_minus_1_rank(enum square from_sq);
enum square sq_get_square_plus_2_rank(enum square from_sq);
enum square sq_get_square_minus_2_rank(enum square from_sq);
enum square sq_try_get_sq(enum rank r, enum file f);
char *print_square(enum square sq);
bool validate_square(enum square sq);
bool validate_rank(enum rank rank);
bool validate_file(enum file file);
bool validate_en_pass_sq(enum square sq_to_check);
bool sq_is_valid_rank(enum rank r);
bool sq_is_valid_file(enum file f);

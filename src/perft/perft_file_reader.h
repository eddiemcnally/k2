/*  MIT License
 *
 *  Copyright (c) 2020 Eddie McNally
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
#include <stdint.h>
#include <stdio.h>

#define MAX_FEN_LEN 150
#define PERFT_EPD_MAX_DEPTH 6
#define PERFT_EPD_ROWS 126

// struct to hold details of a perft row
// 4k2r/6K1/8/8/8/8/8/8 w k - 0 1 ;D1 3 ;D2 32 ;D3 134 ;D4 2073 ;D5 10485 ;D6 179869
struct epd_row {
    char fen[MAX_FEN_LEN];
    uint64_t move_cnt[PERFT_EPD_MAX_DEPTH];
};

struct perft_epd {
    struct epd_row rows[PERFT_EPD_ROWS];
};

struct perft_epd perft_load_file(const char *file);

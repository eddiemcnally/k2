/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */


#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "piece.h"


// enum value : zero = WHITE (to match lsb of piece)
enum colour {
        // colours
        WHITE = 0,
        BLACK,
};


// lsb is colour, 0 = WHITE
enum piece {
        WPAWN = 0,
        BPAWN,
        WBISHOP,
        BBISHOP,
        WKNIGHT,
        BKNIGHT,
        WROOK,
        BROOK,
        WQUEEN,
        BQUEEN,
        WKING,
        BKING        
};

#define         NUM_COLOURS     2
#define         NUM_PIECES      12


bool pce_is_white ( const enum piece pce );
bool pce_is_black ( const enum piece pce );
enum colour swap_side ( const enum colour side );
enum colour pce_get_colour ( const enum piece pce );
uint32_t pce_get_value ( const enum piece pce );
char pce_get_label ( const enum piece pce );
enum piece pce_get_from_label ( const char c );
uint8_t pce_get_array_idx ( const enum piece pce );
uint8_t pce_col_get_array_idx ( const enum colour col );
enum piece pce_get_no_piece ( void );
bool validate_piece ( const enum piece pce );
bool validate_colour ( const enum colour col );
bool validate_label ( const char c );

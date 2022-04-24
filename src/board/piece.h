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

#include "piece.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// clang-format off

enum colour { 
    WHITE,          // must be zero, derived from even piece types
    BLACK 
};

#define NUM_COLOURS (2)


#define COLOUR_AS_ARRAY_OFFSET(colour)  ((uint8_t)colour)
#define COLOUR_AS_ARRAY_OFFSET_WHITE    (COLOUR_AS_ARRAY_OFFSET(WHITE))
#define COLOUR_AS_ARRAY_OFFSET_BLACK    (COLOUR_AS_ARRAY_OFFSET(BLACK))

#define PIECE_AS_ARRAY_OFFSET(pce)      ((uint8_t)pce)

// white is even, black is odd
enum piece {
    WHITE_PAWN,
    BLACK_PAWN,

    WHITE_BISHOP,
    BLACK_BISHOP,

    WHITE_KNIGHT,
    BLACK_KNIGHT,

    WHITE_ROOK,
    BLACK_ROOK,

    WHITE_QUEEN,
    BLACK_QUEEN,

    WHITE_KING, 
    BLACK_KING,

    NO_PIECE
};

#define NUM_PIECE_TYPES (12)

// clang-format on

bool pce_is_white(const enum piece pce);
bool pce_is_black(const enum piece pce);

enum colour pce_swap_side(const enum colour side);
enum colour pce_get_colour(const enum piece pce);
uint32_t pce_get_value(const enum piece pce);
char pce_get_label(const enum piece pce);
enum piece pce_get_from_label(const char c);
bool validate_piece(const enum piece pce);
bool validate_colour(const enum colour col);
bool validate_label(const char c);
void pce_get_all_pieces(enum piece pce_array[NUM_PIECE_TYPES]);

bool pce_is_king(const enum piece pce);
bool pce_is_rook(const enum piece pce);
bool pce_is_pawn(const enum piece pce);
bool pce_is_bishop(const enum piece pce);
bool pce_is_knight(const enum piece pce);
bool pce_is_queen(const enum piece pce);

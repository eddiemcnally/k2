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

enum colour {
    WHITE = 0,
    BLACK,
};

struct piece {
    uint8_t pce_val;
};

enum piece_type { PAWN = 0, BISHOP, KNIGHT, ROOK, QUEEN, KING };

#define NUM_COLOURS 2
#define NUM_PIECE_TYPES 6
#define NUM_PIECES (NUM_PIECE_TYPES * NUM_COLOURS)

struct piece pce_create(const enum piece_type pce_type, const enum colour col);
enum piece_type pce_get_piece_type(const struct piece pce);
bool pce_are_equal(const struct piece pce1, const struct piece pce2);
bool pce_is_white(const struct piece pce);
bool pce_is_black(const struct piece pce);
enum colour pce_swap_side(const enum colour side);
enum colour pce_get_colour(const struct piece pce);
uint32_t pce_get_value(const enum piece_type pt);
char pce_get_label(const struct piece pce);
struct piece pce_get_from_label(const char c);
uint8_t pce_get_array_idx(const enum piece_type pt);
uint8_t pce_col_get_array_idx(const enum colour col);
struct piece pce_get_no_piece(void);
bool validate_piece(const struct piece pce);
bool validate_piece_type(const enum piece_type pt);
bool validate_colour(const enum colour col);
bool validate_label(const char c);
void pce_get_all_pieces(struct piece *pce_array);

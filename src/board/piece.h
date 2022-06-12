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

#define COLOUR_AS_ARRAY_OFFSET(colour)  ((uint8_t)(colour))
#define COLOUR_AS_ARRAY_OFFSET_WHITE    (COLOUR_AS_ARRAY_OFFSET(WHITE))
#define COLOUR_AS_ARRAY_OFFSET_BLACK    (COLOUR_AS_ARRAY_OFFSET(BLACK))

enum piece_role {
    PAWN,
    BISHOP,
    KNIGHT,
    ROOK,
    QUEEN,
    KING,
};

struct piece;

#define NUM_PIECES (12)

// clang-format on

enum colour pce_swap_side(const enum colour side);

uint32_t pce_get_value(const struct piece *const piece);
enum colour pce_get_colour(const struct piece *const piece);
enum piece_role pce_get_role(const struct piece *const piece);
uint8_t pce_get_array_offset(const struct piece *const piece);

const struct piece *pce_get_from_label(const char c);
const struct piece *pce_get(const enum piece_role role, const enum colour colour);

const struct piece *pce_get_white_pawn(void);
const struct piece *pce_get_black_pawn(void);
const struct piece *pce_get_white_bishop(void);
const struct piece *pce_get_black_bishop(void);
const struct piece *pce_get_white_knight(void);
const struct piece *pce_get_black_knight(void);
const struct piece *pce_get_white_rook(void);
const struct piece *pce_get_black_rook(void);
const struct piece *pce_get_white_queen(void);
const struct piece *pce_get_black_queen(void);
const struct piece *pce_get_white_king(void);
const struct piece *pce_get_black_king(void);

char pce_get_label(const struct piece *const piece);

bool validate_piece(const struct piece *const piece);
bool validate_colour(const enum colour col);
bool validate_piece_role(const enum piece_role role);
bool validate_label(const char label);

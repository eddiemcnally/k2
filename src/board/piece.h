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

/* Piece values
 * values taken from here: 
 * https://www.chessprogramming.org/Simplified_Evaluation_Function 
 */
enum piece_values {
    PCE_VAL_PAWN    = 100,
    PCE_VAL_BISHOP  = 330,
    PCE_VAL_KNIGHT  = 320,
    PCE_VAL_ROOK    = 500,
    PCE_VAL_QUEEN   = 900,
    PCE_VAL_KING    = 20000
};

enum piece_role {
    PAWN,
    BISHOP,
    KNIGHT,
    ROOK,
    QUEEN,
    KING,
};

// Piece bitmap (32 bits)
// ---- ---- ---- ---- ---- ---- ---- -XXX      role
// ---- ---- ---- ---- ---- ---- ---- X---      colour
// ---- ---- ---- ---- ---- ---- XXXX ----      piece array offset
// ---- ---- ---- ---- XXXX XXXX ---- ----      unused
// XXXX XXXX XXXX XXXX ---- ---- ---- ----      piece value

#define PCE_MASK_ROLE       (0x00000007)
#define PCE_MASK_COLOUR     (0x00000008)
#define PCE_MASK_OFFSET     (0x000000F0)
#define PCE_MASK_VALUE      (0xFFFF0000)

#define PCE_SHIFT_ROLE      0
#define PCE_SHIFT_COLOUR    3
#define PCE_SHIFT_OFFSET    4
#define PCE_SHIFT_VALUE     16

#define NUM_PIECES          (12)
#define NO_PIECE            (0xFFFFFFFF)

#define PIECE_AS_ARRAY_OFFSET(pce)   ((int)((pce & PCE_MASK_OFFSET) >> PCE_SHIFT_OFFSET))

#define GEN_PIECE(role, colour, value, offset)  ((uint32_t)(                                          \
                                (uint32_t)(((uint32_t)role   << PCE_SHIFT_ROLE)   & PCE_MASK_ROLE)    \
                              | (uint32_t)(((uint32_t)colour << PCE_SHIFT_COLOUR) & PCE_MASK_COLOUR)  \
                              | (uint32_t)(((uint32_t)offset << PCE_SHIFT_OFFSET) & PCE_MASK_OFFSET)  \
                              | (uint32_t)(((uint32_t)value  << PCE_SHIFT_VALUE)  & PCE_MASK_VALUE)   \
                            ))

enum piece {
    WHITE_PAWN      = GEN_PIECE(PAWN,   WHITE, PCE_VAL_PAWN,    0),
    WHITE_BISHOP    = GEN_PIECE(BISHOP, WHITE, PCE_VAL_BISHOP,  1),   
    WHITE_KNIGHT    = GEN_PIECE(KNIGHT, WHITE, PCE_VAL_KNIGHT,  2),
    WHITE_ROOK      = GEN_PIECE(ROOK,   WHITE, PCE_VAL_ROOK,    3),
    WHITE_QUEEN     = GEN_PIECE(QUEEN,  WHITE, PCE_VAL_QUEEN,   4),
    WHITE_KING      = GEN_PIECE(KING,   WHITE, PCE_VAL_KING,    5),
    BLACK_PAWN      = GEN_PIECE(PAWN,   BLACK, PCE_VAL_PAWN,    6),
    BLACK_BISHOP    = GEN_PIECE(BISHOP, BLACK, PCE_VAL_BISHOP,  7),   
    BLACK_KNIGHT    = GEN_PIECE(KNIGHT, BLACK, PCE_VAL_KNIGHT,  8),
    BLACK_ROOK      = GEN_PIECE(ROOK,   BLACK, PCE_VAL_ROOK,    9),
    BLACK_QUEEN     = GEN_PIECE(QUEEN,  BLACK, PCE_VAL_QUEEN,   10),
    BLACK_KING      = GEN_PIECE(KING,   BLACK, PCE_VAL_KING,    11)      
};

// clang-format on

enum colour pce_swap_side(const enum colour side);

uint32_t pce_get_value(const enum piece pce);
enum colour pce_get_colour(const enum piece pce);
enum piece_role pce_get_role(const enum piece pce);
enum piece pce_get_from_label(const char c);
char pce_get_label(const enum piece pce);

bool validate_piece(const enum piece pce);
bool validate_colour(const enum colour col);
bool validate_piece_role(const enum piece_role role);
bool validate_label(const char label);

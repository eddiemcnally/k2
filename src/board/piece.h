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
// ---- ---- ---- ---- ---- ---X ---- ----      is king
// ---- ---- ---- ---- XXXX XXX- ---- ----      unused
// XXXX XXXX XXXX XXXX ---- ---- ---- ----      piece value

#define PCE_MASK_ROLE       (0x00000007)
#define PCE_MASK_COLOUR     (0x00000008)
#define PCE_MASK_OFFSET     (0x000000F0)
#define PCE_MASK_IS_KING    (0x00000100)
#define PCE_MASK_VALUE      (0xFFFF0000)

#define PCE_SHIFT_ROLE      0
#define PCE_SHIFT_COLOUR    3
#define PCE_SHIFT_OFFSET    4
#define PCE_SHIFT_IS_KING   8
#define PCE_SHIFT_VALUE     16

#define NUM_PIECES          (12)
#define NO_PIECE            (0xFFFFFFFF)

#define PIECE_AS_ARRAY_OFFSET(pce)   ((int)((pce & PCE_MASK_OFFSET) >> PCE_SHIFT_OFFSET))


enum pce_offset{
    WP_OFF,
    WB_OFF,
    WN_OFF,
    WR_OFF,
    WQ_OFF,
    WK_OFF,
    BP_OFF,
    BB_OFF,
    BN_OFF,
    BR_OFF,
    BQ_OFF,
    BK_OFF,       
};

#define GEN_PIECE_BIT_MAP(role, colour, value, offset, is_king)  ((uint32_t)(                                    \
                                (uint32_t)(((uint32_t)role    << PCE_SHIFT_ROLE)     & PCE_MASK_ROLE)    \
                              | (uint32_t)(((uint32_t)colour  << PCE_SHIFT_COLOUR)   & PCE_MASK_COLOUR)  \
                              | (uint32_t)(((uint32_t)offset  << PCE_SHIFT_OFFSET)   & PCE_MASK_OFFSET)  \
                              | (uint32_t)(((uint32_t)value   << PCE_SHIFT_VALUE)    & PCE_MASK_VALUE)   \
                              | (uint32_t)(((uint32_t)is_king << PCE_SHIFT_IS_KING)  & PCE_MASK_IS_KING) \
                              ))

enum piece {
    WHITE_PAWN      = GEN_PIECE_BIT_MAP(PAWN,   WHITE, PCE_VAL_PAWN,    WP_OFF, false),
    WHITE_BISHOP    = GEN_PIECE_BIT_MAP(BISHOP, WHITE, PCE_VAL_BISHOP,  WB_OFF, false),   
    WHITE_KNIGHT    = GEN_PIECE_BIT_MAP(KNIGHT, WHITE, PCE_VAL_KNIGHT,  WN_OFF, false),
    WHITE_ROOK      = GEN_PIECE_BIT_MAP(ROOK,   WHITE, PCE_VAL_ROOK,    WR_OFF, false),
    WHITE_QUEEN     = GEN_PIECE_BIT_MAP(QUEEN,  WHITE, PCE_VAL_QUEEN,   WQ_OFF, false),
    WHITE_KING      = GEN_PIECE_BIT_MAP(KING,   WHITE, PCE_VAL_KING,    WK_OFF, true),
    BLACK_PAWN      = GEN_PIECE_BIT_MAP(PAWN,   BLACK, PCE_VAL_PAWN,    BP_OFF, false),
    BLACK_BISHOP    = GEN_PIECE_BIT_MAP(BISHOP, BLACK, PCE_VAL_BISHOP,  BB_OFF, false),   
    BLACK_KNIGHT    = GEN_PIECE_BIT_MAP(KNIGHT, BLACK, PCE_VAL_KNIGHT,  BN_OFF, false),
    BLACK_ROOK      = GEN_PIECE_BIT_MAP(ROOK,   BLACK, PCE_VAL_ROOK,    BR_OFF, false),
    BLACK_QUEEN     = GEN_PIECE_BIT_MAP(QUEEN,  BLACK, PCE_VAL_QUEEN,   BQ_OFF, false),
    BLACK_KING      = GEN_PIECE_BIT_MAP(KING,   BLACK, PCE_VAL_KING,    BK_OFF, true)      
};

// clang-format on

enum colour pce_swap_side(const enum colour side);

uint32_t pce_get_value(const enum piece pce);
enum colour pce_get_colour(const enum piece pce);
enum piece_role pce_get_role(const enum piece pce);
enum piece pce_get_from_label(const char c);
bool pce_is_king(const enum piece pce);
char pce_get_label(const enum piece pce);

bool validate_piece(const enum piece pce);
bool validate_colour(const enum colour col);
bool validate_piece_role(const enum piece_role role);
bool validate_label(const char label);

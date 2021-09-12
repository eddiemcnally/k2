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
    WHITE,
    BLACK,
};

// piece bitmap (uint32_t)
//
// XXXX XXXX XXXX XXXX ---- ---- ---- ----   Piece Value
// ---- ---- ---- ---- XXXX XXXX ---- ----   Unused
// ---- ---- ---- ---- ---- ---- -XXX ----   Piece Role
// ---- ---- ---- ---- ---- ---- X--- ----   Colour 0-> White, 1 -> Black
// ---- ---- ---- ---- ---- ---- ---- XXXX   Offset when used as an array lookup
//
//
// ---- ---- ---- ---- ---- ---- -000 ----   Pawn
// ---- ---- ---- ---- ---- ---- -001 ----   Bishop
// ---- ---- ---- ---- ---- ---- -010 ----   Knight
// ---- ---- ---- ---- ---- ---- -011 ----   Rook
// ---- ---- ---- ---- ---- ---- -100 ----   Queen
// ---- ---- ---- ---- ---- ---- -101 ----   King
// ---- ---- ---- ---- ---- ---- 1--- ----   BLACK = 1, WHITE = 0
//
//                                           Array offsets
//                                           =============
// ---- ---- ---- ---- ---- ---- ---- 0000   White   Pawn Offset
// ---- ---- ---- ---- ---- ---- ---- 0001           Bishop Offset
// ---- ---- ---- ---- ---- ---- ---- 0010           Knight Offset
// ---- ---- ---- ---- ---- ---- ---- 0011           Rook Offset
// ---- ---- ---- ---- ---- ---- ---- 0100           Queen Offset
// ---- ---- ---- ---- ---- ---- ---- 0101           King Offset
// ---- ---- ---- ---- ---- ---- ---- 0110   Black   Pawn offset
// ---- ---- ---- ---- ---- ---- ---- 0111           Bishop Offset
// ---- ---- ---- ---- ---- ---- ---- 1000           Knight Offset
// ---- ---- ---- ---- ---- ---- ---- 1001           Rook offset
// ---- ---- ---- ---- ---- ---- ---- 1010           Queen Offset
// ---- ---- ---- ---- ---- ---- ---- 1011           King Offset

// clang-format off
enum piece_role { 
    PAWN, 
    BISHOP, 
    KNIGHT, 
    ROOK, 
    QUEEN, 
    KING 
};

#define ROLE_SHIFT  (4)
#define VALUE_SHIFT (16)

enum piece_array_offsets {
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

#define COLOUR_MASK ((uint8_t)0x80)
#define OFFSET_MASK ((uint8_t)0x0F)

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

// constructs the bitmapped piece
#define PCE_CTOR_WHITE(role, offset, val) ((uint32_t)((role << ROLE_SHIFT) | (uint8_t)(offset) | (val << VALUE_SHIFT)))
#define PCE_CTOR_BLACK(role, offset, val)                                                                              \
    ((uint32_t)((role << ROLE_SHIFT) | COLOUR_MASK | (uint8_t)(offset) | (val << VALUE_SHIFT)))

enum piece {
    WHITE_PAWN      = PCE_CTOR_WHITE(PAWN, WP_OFF, PCE_VAL_PAWN),
    WHITE_BISHOP    = PCE_CTOR_WHITE(BISHOP, WB_OFF, PCE_VAL_BISHOP),
    WHITE_KNIGHT    = PCE_CTOR_WHITE(KNIGHT, WN_OFF, PCE_VAL_KNIGHT),
    WHITE_ROOK      = PCE_CTOR_WHITE(ROOK, WR_OFF, PCE_VAL_ROOK),
    WHITE_QUEEN     = PCE_CTOR_WHITE(QUEEN, WQ_OFF, PCE_VAL_QUEEN),
    WHITE_KING      = PCE_CTOR_WHITE(KING, WK_OFF, PCE_VAL_KING),

    BLACK_PAWN      = PCE_CTOR_BLACK(PAWN, BP_OFF, PCE_VAL_PAWN),
    BLACK_BISHOP    = PCE_CTOR_BLACK(BISHOP, BB_OFF, PCE_VAL_BISHOP),
    BLACK_KNIGHT    = PCE_CTOR_BLACK(KNIGHT, BN_OFF, PCE_VAL_KNIGHT),
    BLACK_ROOK      = PCE_CTOR_BLACK(ROOK, BR_OFF, PCE_VAL_ROOK),
    BLACK_QUEEN     = PCE_CTOR_BLACK(QUEEN, BQ_OFF, PCE_VAL_QUEEN),
    BLACK_KING      = PCE_CTOR_BLACK(KING, BK_OFF, PCE_VAL_KING),

    NO_PIECE        = 0x7FFFFFFF
};

#define NUM_COLOURS     (2)
#define NUM_PIECE_ROLES (6)
#define NUM_PIECES      (NUM_PIECE_ROLES * NUM_COLOURS)
#define OFFSET_SHIFT    (0)

#define PCE_GET_ARRAY_INDEX(pce)        ((int)((pce & OFFSET_MASK) >> OFFSET_SHIFT))
#define PCE_COL_GET_ARRAY_INDEX(col)    ((int)(col))

#define PCE_COL_ARRAY_OFFSET_WHITE      ((PCE_COL_GET_ARRAY_INDEX(WHITE)))
#define PCE_COL_ARRAY_OFFSET_BLACK      ((PCE_COL_GET_ARRAY_INDEX(BLACK)))

#define PCE_ARRAY_OFFSET_WHITE_BISHOP   ((PCE_GET_ARRAY_INDEX(WHITE_BISHOP)))
#define PCE_ARRAY_OFFSET_WHITE_QUEEN    ((PCE_GET_ARRAY_INDEX(WHITE_QUEEN)))
#define PCE_ARRAY_OFFSET_WHITE_ROOK     ((PCE_GET_ARRAY_INDEX(WHITE_ROOK)))
#define PCE_ARRAY_OFFSET_WHITE_KING     ((PCE_GET_ARRAY_INDEX(WHITE_KING)))

#define PCE_ARRAY_OFFSET_BLACK_BISHOP   ((PCE_GET_ARRAY_INDEX(BLACK_BISHOP)))
#define PCE_ARRAY_OFFSET_BLACK_QUEEN    ((PCE_GET_ARRAY_INDEX(BLACK_QUEEN)))
#define PCE_ARRAY_OFFSET_BLACK_ROOK     ((PCE_GET_ARRAY_INDEX(BLACK_ROOK)))
#define PCE_ARRAY_OFFSET_BLACK_KING     ((PCE_GET_ARRAY_INDEX(BLACK_KING)))

// clang-format on

enum piece_role pce_get_piece_role(const enum piece pce);
bool pce_is_white(const enum piece pce);
bool pce_is_black(const enum piece pce);
enum colour pce_swap_side(const enum colour side);
enum colour pce_get_colour(const enum piece pce);
uint32_t pce_get_value(const enum piece pce);
char pce_get_label(const enum piece pce);
enum piece pce_get_from_label(const char c);
bool validate_piece(const enum piece pce);
bool validate_piece_role(const enum piece_role pt);
bool validate_colour(const enum colour col);
bool validate_label(const char c);
void pce_get_all_pieces(enum piece pce_array[NUM_PIECES]);
bool pce_is_king(const enum piece pce);

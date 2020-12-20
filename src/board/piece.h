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
    BLACK = 1,
};

// -XXX ----   ROLE
// X--- ----   Colour 0-> White, 1 -> Black
// ---- XXXX   Offset when used in an array
//===========================================
// -000 ----   Pawn
// -001 ----   Bishop
// -010 ----   Knight
// -011 ----   Rook
// -100 ----   Queen
// -101 ----   King
// 1--- ----   BLACK = 1, WHITE = 0
// ---- 0000   White   Pawn Offset
// ---- 0001           Bishop Offset
// ---- 0010           Knight Offset
// ---- 0011           Rook Offset
// ---- 0100           Queen Offset
// ---- 0101           King Offset
// ---- 0110   Black   Pawn offset
// ---- 0111           Bishop Offset
// ---- 1000           Knight Offset
// ---- 1001           Rook offset
// ---- 1010           Queen Offset
// ---- 1011           King Offset

enum piece_role { PAWN = 0x00, BISHOP = 0x10, KNIGHT = 0x20, ROOK = 0x30, QUEEN = 0x40, KING = 0x50 };

#define ROLE_AS_INDEX(r) ((uint8_t)(r >> 4))

enum {
    WP_OFF = 0x00,
    WB_OFF = 0x01,
    WN_OFF = 0x02,
    WR_OFF = 0x03,
    WQ_OFF = 0x04,
    WK_OFF = 0x05,
    BP_OFF = 0x06,
    BB_OFF = 0x07,
    BN_OFF = 0x08,
    BR_OFF = 0x09,
    BQ_OFF = 0x0A,
    BK_OFF = 0x0B,
};

#define COLOUR_MASK ((uint8_t)0x80)
#define OFFSET_MASK ((uint8_t)0x0F)

enum piece {
    WHITE_PAWN = (uint8_t)(PAWN | (uint8_t)(WP_OFF)),
    WHITE_BISHOP = (uint8_t)(BISHOP | (uint8_t)(WB_OFF)),
    WHITE_KNIGHT = (uint8_t)(KNIGHT | (uint8_t)(WN_OFF)),
    WHITE_ROOK = (uint8_t)(ROOK | (uint8_t)(WR_OFF)),
    WHITE_QUEEN = (uint8_t)(QUEEN | (uint8_t)(WQ_OFF)),
    WHITE_KING = (uint8_t)(KING | (uint8_t)(WK_OFF)),

    BLACK_PAWN = (uint8_t)(PAWN | COLOUR_MASK | (uint8_t)(BP_OFF)),
    BLACK_BISHOP = (uint8_t)(BISHOP | COLOUR_MASK | (uint8_t)(BB_OFF)),
    BLACK_KNIGHT = (uint8_t)(KNIGHT | COLOUR_MASK | (uint8_t)(BN_OFF)),
    BLACK_ROOK = (uint8_t)(ROOK | COLOUR_MASK | (uint8_t)(BR_OFF)),
    BLACK_QUEEN = (uint8_t)(QUEEN | COLOUR_MASK | (uint8_t)(BQ_OFF)),
    BLACK_KING = (uint8_t)(KING | COLOUR_MASK | (uint8_t)(BK_OFF)),

    NO_PIECE = (uint8_t)0xFF
};

#define NUM_COLOURS (2)
#define NUM_PIECE_ROLES (6)
#define NUM_PIECES (NUM_PIECE_ROLES * NUM_COLOURS)

#define OFFSET_SHIFT (0)
#define PCE_GET_ARRAY_INDEX(pce) ((uint8_t)((pce & OFFSET_MASK) >> OFFSET_SHIFT))
#define PCE_COL_GET_ARRAY_INDEX(col) ((uint8_t)(col))

#define PCE_COL_ARRAY_OFFSET_WHITE ((PCE_COL_GET_ARRAY_INDEX(WHITE)))
#define PCE_COL_ARRAY_OFFSET_BLACK ((PCE_COL_GET_ARRAY_INDEX(BLACK)))

#define PCE_ARRAY_OFFSET_WHITE_BISHOP ((PCE_GET_ARRAY_INDEX(WHITE_BISHOP)))
#define PCE_ARRAY_OFFSET_WHITE_QUEEN ((PCE_GET_ARRAY_INDEX(WHITE_QUEEN)))
#define PCE_ARRAY_OFFSET_WHITE_ROOK ((PCE_GET_ARRAY_INDEX(WHITE_ROOK)))
#define PCE_ARRAY_OFFSET_WHITE_KING ((PCE_GET_ARRAY_INDEX(WHITE_KING)))

#define PCE_ARRAY_OFFSET_BLACK_BISHOP ((PCE_GET_ARRAY_INDEX(BLACK_BISHOP)))
#define PCE_ARRAY_OFFSET_BLACK_QUEEN ((PCE_GET_ARRAY_INDEX(BLACK_QUEEN)))
#define PCE_ARRAY_OFFSET_BLACK_ROOK ((PCE_GET_ARRAY_INDEX(BLACK_ROOK)))
#define PCE_ARRAY_OFFSET_BLACK_KING ((PCE_GET_ARRAY_INDEX(BLACK_KING)))

enum piece_role pce_get_piece_role(const enum piece pce);
bool pce_is_white(const enum piece pce);
bool pce_is_black(const enum piece pce);
enum colour pce_swap_side(const enum colour side);
enum colour pce_get_colour(const enum piece pce);
uint32_t pce_get_value(const enum piece_role pt);
char pce_get_label(const enum piece pce);
enum piece pce_get_from_label(const char c);
bool validate_piece(const enum piece pce);
bool validate_piece_role(const enum piece_role pt);
bool validate_colour(const enum colour col);
bool validate_label(const char c);
void pce_get_all_pieces(enum piece pce_array[NUM_PIECES]);

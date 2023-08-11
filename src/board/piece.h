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

#include "score.h"

// clang-format off

enum colour { 
    WHITE,          // must be zero, derived from even piece types
    BLACK 
};

#define COLOUR_AS_ARRAY_OFFSET(colour)  ((size_t)colour)
#define ROLE_AS_ARRAY_OFFSET(role)      ((size_t)role)


#define NUM_COLOURS (2)

enum piece_role {
    PAWN    = 0x00,
    BISHOP  = 0x01,
    KNIGHT  = 0x02,
    ROOK    = 0x03,
    QUEEN   = 0x04,
    KING    = 0x05,
};


#define NUM_PIECES          (12)
#define NUM_PIECE_ROLES     (6)
#define NO_PIECE            (0xFF)

#define PCE_COL_MASK        (0x80)


enum piece {
    WHITE_PAWN      = PAWN,
    WHITE_BISHOP    = BISHOP,   
    WHITE_KNIGHT    = KNIGHT,
    WHITE_ROOK      = ROOK,
    WHITE_QUEEN     = QUEEN,
    WHITE_KING      = KING,
    BLACK_PAWN      = (PAWN | PCE_COL_MASK),
    BLACK_BISHOP    = (BISHOP | PCE_COL_MASK),   
    BLACK_KNIGHT    = (KNIGHT | PCE_COL_MASK),
    BLACK_ROOK      = (ROOK | PCE_COL_MASK),
    BLACK_QUEEN     = (QUEEN | PCE_COL_MASK),
    BLACK_KING      = (KING | PCE_COL_MASK)      
};

// clang-format on

enum colour pce_swap_side(const enum colour side);
enum piece_role pce_get_role(const enum piece pce);
Score pce_get_value(const enum piece pce);
enum colour pce_get_colour(const enum piece pce);
enum piece_role pce_get_role(const enum piece pce);
enum piece pce_get_from_label(const char c);
bool pce_is_king(const enum piece pce);
char pce_get_label(const enum piece pce);

bool validate_piece(const enum piece pce);
bool validate_colour(const enum colour col);
bool validate_piece_role(const enum piece_role role);
bool validate_label(const char label);

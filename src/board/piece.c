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

/*! @addtogroup Piece
 *
 * @ingroup Piece
 * @{
 * @details This contains code associated with a piece, colour, value, role, etc.
 *
 */

#include "piece.h"
#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

//
// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief       Takes a given colour/side, and returns the other colour/side
 *
 * @param col   The given colour
 * @return      The opposite colour
 */
enum colour pce_swap_side(const enum colour col) {
    assert(validate_colour(col));

    return (enum colour)(~col & 0x01);
}

__attribute__((always_inline)) uint32_t pce_get_value(const enum piece pce) {
    return (uint32_t)(((uint32_t)pce & PCE_MASK_VALUE) >> PCE_SHIFT_VALUE);
}

__attribute__((always_inline)) enum colour pce_get_colour(const enum piece pce) {
    return (enum colour)((pce & PCE_MASK_COLOUR) >> PCE_SHIFT_COLOUR);
}

__attribute__((always_inline)) enum piece_role pce_get_role(const enum piece pce) {
    return (enum piece_role)((pce & PCE_MASK_ROLE) >> PCE_SHIFT_ROLE);
}

char pce_get_label(const enum piece pce) {
    switch (pce) {
    case WHITE_PAWN:
        return 'P';
    case WHITE_BISHOP:
        return 'B';
    case WHITE_KNIGHT:
        return 'N';
    case WHITE_ROOK:
        return 'R';
    case WHITE_QUEEN:
        return 'Q';
    case WHITE_KING:
        return 'K';
    case BLACK_PAWN:
        return 'p';
    case BLACK_BISHOP:
        return 'b';
    case BLACK_KNIGHT:
        return 'n';
    case BLACK_ROOK:
        return 'r';
    case BLACK_QUEEN:
        return 'q';
    case BLACK_KING:
        return 'k';
    default:
        REQUIRE(false, "invalid piece");
        return 0;
    }
}

enum piece pce_get_from_label(const char c) {

    switch (c) {
    case 'P':
        return WHITE_PAWN;
    case 'B':
        return WHITE_BISHOP;
    case 'N':
        return WHITE_KNIGHT;
    case 'R':
        return WHITE_ROOK;
    case 'Q':
        return WHITE_QUEEN;
    case 'K':
        return WHITE_KING;
    case 'p':
        return BLACK_PAWN;
    case 'b':
        return BLACK_BISHOP;
    case 'n':
        return BLACK_KNIGHT;
    case 'r':
        return BLACK_ROOK;
    case 'q':
        return BLACK_QUEEN;
    case 'k':
        return BLACK_KING;
    default:
        REQUIRE(false, "Invalid piece  label");
    }
}

/**
 * @brief       Validates a colour is within expected range of values
 *
 * @param col The colour
 */
bool validate_colour(const enum colour col) {
    switch (col) {
    case WHITE:
    case BLACK:
        return true;
    default:
        return false;
    }
}

bool validate_piece_role(const enum piece_role role) {
    switch (role) {
    case PAWN:
    case BISHOP:
    case KNIGHT:
    case ROOK:
    case QUEEN:
    case KING:
        return true;
    default:
        return false;
    }
}

bool validate_label(const char label) {
    switch (label) {
    case 'P':
    case 'B':
    case 'N':
    case 'R':
    case 'Q':
    case 'K':
    case 'p':
    case 'b':
    case 'n':
    case 'r':
    case 'q':
    case 'k':
        return true;
    default:
        return false;
    }
}

bool validate_piece(const enum piece pce) {
    switch (pce) {
    case WHITE_PAWN:
    case WHITE_BISHOP:
    case WHITE_KNIGHT:
    case WHITE_ROOK:
    case WHITE_QUEEN:
    case WHITE_KING:
    case BLACK_PAWN:
    case BLACK_BISHOP:
    case BLACK_KNIGHT:
    case BLACK_ROOK:
    case BLACK_QUEEN:
    case BLACK_KING:
        return true;
    default:
        return false;
    }
}

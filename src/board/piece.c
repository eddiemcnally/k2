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

/* Piece values
 * values taken from here: 
 * https://www.chessprogramming.org/Simplified_Evaluation_Function 
 */
// clang-format off
enum piece_values {
    PCE_VAL_PAWN    = 100,
    PCE_VAL_BISHOP  = 330,
    PCE_VAL_KNIGHT  = 320,
    PCE_VAL_ROOK    = 500,
    PCE_VAL_QUEEN   = 900,
    PCE_VAL_KING    = 20000
};

#define COL_MASK (0x01)

// clang-format on
//
// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief       Tests if the given piece is WHITE
 *
 * @param pce   The piece
 * @return true if WHITE, false otherwise
 */
bool pce_is_white(const enum piece pce) {
    assert(validate_piece(pce));

    return (pce & COL_MASK) == 0;
}

/**
 * @brief       Tests if the given piece is BLACK
 *
 * @param pce   The piece
 * @return true if BLACK, false otherwise
 */
bool pce_is_black(const enum piece pce) {
    assert(validate_piece(pce));

    return (pce & COL_MASK) != 0;
}

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

/**
 * @brief       Gets the colour associated with the given piece
 *
 * @param pce The piece
 * @return The colour of the given piece
 */
enum colour pce_get_colour(const enum piece pce) {
    assert(validate_piece(pce));

    return (enum colour)(pce & COL_MASK);
}

/**
 * @brief       Gets the piece value of the given piece
 *
 * @param pce   The piece
 * @return      The piece value
 */
uint32_t pce_get_value(const enum piece pce) {
    assert(validate_piece(pce));

    switch (pce) {
    case WHITE_PAWN:
    case BLACK_PAWN:
        return PCE_VAL_PAWN;
    case WHITE_BISHOP:
    case BLACK_BISHOP:
        return PCE_VAL_BISHOP;
    case WHITE_KNIGHT:
    case BLACK_KNIGHT:
        return PCE_VAL_KNIGHT;
    case WHITE_ROOK:
    case BLACK_ROOK:
        return PCE_VAL_ROOK;
    case WHITE_QUEEN:
    case BLACK_QUEEN:
        return PCE_VAL_QUEEN;
    case WHITE_KING:
    case BLACK_KING:
        return PCE_VAL_KING;
    case NO_PIECE:
        REQUIRE(false, "Invalid use of NO_PIECE");
    default:
        REQUIRE(false, "Invalid piece");
    }
}

bool pce_is_king(const enum piece pce) {
    assert(validate_piece(pce));
    return (pce == WHITE_KING) || (pce == BLACK_KING);
}
bool pce_is_rook(const enum piece pce) {
    assert(validate_piece(pce));
    return (pce == WHITE_ROOK) || (pce == BLACK_ROOK);
}
bool pce_is_pawn(const enum piece pce) {
    assert(validate_piece(pce));
    return (pce == WHITE_PAWN) || (pce == BLACK_PAWN);
}
bool pce_is_bishop(const enum piece pce) {
    assert(validate_piece(pce));
    return (pce == WHITE_BISHOP) || (pce == BLACK_BISHOP);
}
bool pce_is_knight(const enum piece pce) {
    assert(validate_piece(pce));
    return (pce == WHITE_KNIGHT) || (pce == BLACK_KNIGHT);
}
bool pce_is_queen(const enum piece pce) {
    assert(validate_piece(pce));
    return (pce == WHITE_QUEEN) || (pce == BLACK_QUEEN);
}

/**
* @brief        Populates an array of all pieces.
* @param  pce_array      Pointer to array that is populated
*/
void pce_get_all_pieces(enum piece pce_array[NUM_PIECE_TYPES]) {
    int i = 0;

    pce_array[i++] = WHITE_PAWN;
    pce_array[i++] = WHITE_BISHOP;
    pce_array[i++] = WHITE_KNIGHT;
    pce_array[i++] = WHITE_ROOK;
    pce_array[i++] = WHITE_QUEEN;
    pce_array[i++] = WHITE_KING;

    pce_array[i++] = BLACK_PAWN;
    pce_array[i++] = BLACK_BISHOP;
    pce_array[i++] = BLACK_KNIGHT;
    pce_array[i++] = BLACK_ROOK;
    pce_array[i++] = BLACK_QUEEN;
    pce_array[i++] = BLACK_KING;
}

/**
 * @brief       Gets the piece lable for the given piece (eg, 'P' for white pawn, 'k' for black king)
 *
 * @param pce The piece
 * @return A character representing the labe
 */
char pce_get_label(const enum piece pce) {
    assert(validate_piece(pce));

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
    case NO_PIECE:
    default:
        REQUIRE(false, "Invalid Piece");
    }
}

/**
 * @brief       For the given piece label, returns the associated enum piece
 *
 * @param c The piece label
 * @return The associated piece
 */
enum piece pce_get_from_label(const char c) {
    assert(validate_label(c));

    enum piece retval = NO_PIECE;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

    switch (c) {
    case 'P':
        return WHITE_PAWN;
    case 'R':
        return WHITE_ROOK;
    case 'N':
        return WHITE_KNIGHT;
    case 'B':
        return WHITE_BISHOP;
    case 'Q':
        return WHITE_QUEEN;
    case 'K':
        return WHITE_KING;
    case 'p':
        return BLACK_PAWN;
    case 'r':
        return BLACK_ROOK;
    case 'n':
        return BLACK_KNIGHT;
    case 'b':
        return BLACK_BISHOP;
    case 'q':
        return BLACK_QUEEN;
    case 'k':
        return BLACK_KING;
    case NO_PIECE:
    default:
        REQUIRE(false, "Invalid Piece label char");
    }

#pragma GCC diagnostic pop
    return retval;
}

/**
 * @brief       Validates a piece is within expected range of values
 *
 * @param pce The piece
 */
bool validate_piece(const enum piece pce) {
    if (pce == NO_PIECE) {
        return true;
    }

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

    case NO_PIECE:
    default:
        return false;
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

/**
 * @brief       Validates a piece label is within a valid range of values
 *
 * @param c The piece label
 */
bool validate_label(const char c) {

    char cu = (char)toupper(c);

    switch (cu) {
    case 'P':
    case 'R':
    case 'N':
    case 'B':
    case 'Q':
    case 'K':
        return true;
    default:
        return false;
    }
}

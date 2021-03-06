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

#define ROLE_MASK ((uint8_t)0x70)
#define COLOUR_SHIFT (7)

#define EXTRACT_COLOUR(pce) ((enum colour)((pce & COLOUR_MASK) >> COLOUR_SHIFT))
#define EXTRACT_PIECE_ROLE(pce) ((enum piece_role)((pce & ROLE_MASK) >> ROLE_SHIFT))

// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief           Returns the piece_role  
 *
 * @param pce       The piece
 * 
 * @return          the piece_type
 */
inline enum piece_role pce_get_piece_role(const enum piece pce) {
    assert(validate_piece(pce));

    return EXTRACT_PIECE_ROLE(pce);
}

/**
 * @brief       Tests if the given piece is WHITE
 *
 * @param pce   The piece
 * @return true if WHITE, false otherwise
 */
inline bool pce_is_white(const enum piece pce) {
    assert(validate_piece(pce));

    return (EXTRACT_COLOUR(pce) == WHITE);
}

/**
 * @brief       Tests if the given piece is BLACK
 *
 * @param pce   The piece
 * @return true if BLACK, false otherwise
 */
inline bool pce_is_black(const enum piece pce) {
    assert(validate_piece(pce));

    return (EXTRACT_COLOUR(pce) == BLACK);
}

/**
 * @brief       Takes a given colour/side, and returns the other colour/side
 *
 * @param col   The given colour
 * @return      The opposite colour
 */
inline enum colour pce_swap_side(const enum colour col) {
    assert(validate_colour(col));

    return (enum colour)(~col & 0x01);
}

/**
 * @brief       Gets the colour associated with the given piece
 *
 * @param pce The piece
 * @return The colour of the given piece
 */
inline enum colour pce_get_colour(const enum piece pce) {
    assert(validate_piece(pce));

    return EXTRACT_COLOUR(pce);
}

/**
 * @brief       Gets the piece value of the given piece
 *
 * @param pce   The piece
 * @return      The piece value
 */
inline uint32_t pce_get_value(const enum piece pce) {
    assert(validate_piece(pce));

    return (uint32_t)(pce >> VALUE_SHIFT);
}

bool pce_is_king(const enum piece pce) {
    return ((pce >> ROLE_SHIFT) & KING);
}

/**
* @brief        Populates an array of all pieces.
* @param        Pointer to array that is populated
* @return       The array
*/
inline void pce_get_all_pieces(enum piece pce_array[NUM_PIECES]) {
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

    enum colour col = EXTRACT_COLOUR(pce);
    enum piece_role pt = EXTRACT_PIECE_ROLE(pce);

    char retval;

    switch (pt) {
    case PAWN:
        retval = 'p';
        break;
    case BISHOP:
        retval = 'b';
        break;
    case KNIGHT:
        retval = 'n';
        break;
    case ROOK:
        retval = 'r';
        break;
    case QUEEN:
        retval = 'q';
        break;
    case KING:
        retval = 'k';
        break;
    default:
        REQUIRE(false, "Invalid Piece");
    }

    if (col == WHITE) {
        retval = (char)toupper(retval);
    }

    return retval;
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
    const enum piece_role pt = EXTRACT_PIECE_ROLE(pce);
    assert(validate_piece_role(pt));
    const enum colour col = EXTRACT_COLOUR(pce);
    assert(validate_colour(col));

    switch (pt) {
    case PAWN:
    case KNIGHT:
    case BISHOP:
    case ROOK:
    case QUEEN:
    case KING:
        break;
    default:
        assert(false);
    }

    validate_colour(col);

    return true;
}

/**
 * @brief       Validates a piece type within expected range of values
 *
 * @param pt    The piece type
 */
bool validate_piece_role(const enum piece_role pt) {

    switch (pt) {
    case PAWN:
    case KNIGHT:
    case BISHOP:
    case ROOK:
    case QUEEN:
    case KING:
        break;
    default:
        assert(false);
        break;
    }
    return true;
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
        assert(false);
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
        printf("Invalid label %c\n", c);
        assert(false);
    }
    return false;
}

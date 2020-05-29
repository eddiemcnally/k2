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
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

/**
 * Piece values
 * values taken from here: 
 * https://www.chessprogramming.org/Simplified_Evaluation_Function 
 */
enum piece_values {
    PCE_VAL_PAWN = 100,
    PCE_VAL_BISHOP = 330,
    PCE_VAL_KNIGHT = 320,
    PCE_VAL_ROOK = 500,
    PCE_VAL_QUEEN = 900,
    PCE_VAL_KING = 20000
};
#define NO_PIECE ((uint8_t)0xFF)

#define COLOUR_SHIFT 3
#define OFFSET_SHIFT 4

static enum colour extract_colour(const enum piece pce);
static enum piece_role extract_piece_role(const enum piece pce);

static_assert(WHITE_OFFSET == WHITE, "Colour WHITE offset incorrect");
static_assert(BLACK_OFFSET == BLACK, "Colour BLACK offset incorrect");

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
enum piece_role pce_get_piece_role(const enum piece pce) {
    assert(validate_piece(pce));

    return extract_piece_role(pce);
}

/**
 * @brief       Tests if the given piece is WHITE
 *
 * @param pce   The piece
 * @return true if WHITE, false otherwise
 */
bool pce_is_white(const enum piece pce) {
    assert(validate_piece(pce));

    enum colour col = extract_colour(pce);
    return col == WHITE;
}

/**
 * @brief       Tests if the given piece is BLACK
 *
 * @param pce   The piece
 * @return true if BLACK, false otherwise
 */
bool pce_is_black(const enum piece pce) {
    assert(validate_piece(pce));

    enum colour col = extract_colour(pce);
    return col == BLACK;
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

    return extract_colour(pce);
}

/**
* @brief Returns an enum value that can be used to denote "no piece"
*
* @return piece A "no piece" enum
*/
inline enum piece pce_get_no_piece(void) { return (enum piece)NO_PIECE; }

/**
 * @brief       Gets the piece value of the given piece_type
 *
 * @param pt    The piece_type
 * @return      The piece value
 */
uint32_t pce_get_value(const enum piece_role pt) {
    assert(validate_piece_role(pt));

    switch (pt) {
    case PAWN:
        return PCE_VAL_PAWN;
    case BISHOP:
        return PCE_VAL_BISHOP;
    case KNIGHT:
        return PCE_VAL_KNIGHT;
    case ROOK:
        return PCE_VAL_ROOK;
    case QUEEN:
        return PCE_VAL_QUEEN;
    case KING:
        return PCE_VAL_KING;
    default:
        assert(false);
        break;
    }
}

/**
* @brief            Converts the piece-type to an array index for use by various modules
*
* @param pt         The piece
* @return           uint8_t The array index
*/
uint8_t pce_get_array_idx(const enum piece pce) {
    assert(validate_piece(pce));

    return (pce & OFFSET_MASK) >> OFFSET_SHIFT;
}

/**
* @brief        Converts ths given colour to an array index.
*
* @param col    p_col The colour
* @return uint8_t The array index
*/
uint8_t pce_col_get_array_idx(const enum colour col) {
    assert(validate_colour(col));

    return (uint8_t)col;
}

/**
* @brief        Populates an array of all pieces.
* @param        Pointer to array that is populated
* @return       The array
*/
void pce_get_all_pieces(enum piece pce_array[NUM_PIECES]) {
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

    enum colour col = extract_colour(pce);
    enum piece_role pt = extract_piece_role(pce);

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
        assert(false);
        return '-';
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
        assert(false);
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
    const enum piece_role pt = extract_piece_role(pce);
    assert(validate_piece_role(pt));
    const enum colour col = extract_colour(pce);
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

static enum colour extract_colour(const enum piece pce) {
    return (enum colour)((pce & COLOUR_MASK) >> COLOUR_SHIFT);
}

static enum piece_role extract_piece_role(const enum piece pce) {
    enum piece_role r = pce & ROLE_MASK;
    assert(validate_piece_role(r));

    return (enum piece_role)(r);
}

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

#include "piece.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

/**
 * Piece values
 */
enum piece_values {
    PCE_VAL_PAWN = 100,
    PCE_VAL_BISHOP = 325,
    PCE_VAL_KNIGHT = 325,
    PCE_VAL_ROOK = 500,
    PCE_VAL_QUEEN = 1000,
    PCE_VAL_KING = 50000
};

#define COLOUR_MASK 0x80
#define COLOUR_SHIFT 7
#define NO_PIECE 0xFF

static enum colour extract_colour(const struct piece pce);
static enum piece_type extract_piece_type(const struct piece pce);

// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief           Creates a piece 
 *
 * @param pce_type  The piece
 * @param col       The colour
 * 
 * @return          the constructed piece
 */
struct piece pce_create(const enum piece_type pce_type, const enum colour col) {
    uint8_t p = pce_type;

    switch (col) {
    case WHITE:
        break;
    case BLACK:
        p = (uint8_t)(p | COLOUR_MASK);
        break;
    default:
        assert(false);
    }

    struct piece retval = {.pce_val = p};

    return retval;
}

/**
 * @brief           Returns the piece_type  
 *
 * @param pce       The piece
 * 
 * @return          the piece_type
 */

enum piece_type pce_get_piece_type(const struct piece pce) {
    assert(validate_piece(pce));

    return extract_piece_type(pce);
}

/**
 * @brief       Tests if the given piece is WHITE
 *
 * @param pce   The piece
 * @return true if WHITE, false otherwise
 */
bool pce_is_white(const struct piece pce) {
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
bool pce_is_black(const struct piece pce) {
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

    switch (col) {
    case WHITE:
        return BLACK;
    case BLACK:
        return WHITE;
    default:
        assert(false);
    }
}

/**
 * @brief       Gets the colour associated with the given piece
 *
 * @param pce The piece
 * @return The colour of the given piece
 */
enum colour pce_get_colour(const struct piece pce) {
    assert(validate_piece(pce));

    return extract_colour(pce);
}

/**
* @brief Returns an enum value that can be used to denote "no piece"
*
* @return piece A "no piece" enum
*/
struct piece pce_get_no_piece(void) {
    struct piece pce = {.pce_val = NO_PIECE};
    return pce;
}

/**
 * @brief       Compares 2 pieces for equality
 *
 * @param pce1  first piece
 * @param pce2  second piece
 * 
 * @return      true if same, false otherwise
 */

bool pce_are_equal(const struct piece pce1, const struct piece pce2) {
    return pce1.pce_val == pce2.pce_val;
}

/**
 * @brief       Gets the piece value of the given piece_type
 *
 * @param pt    The piece_type
 * @return      The piece value
 */
uint32_t pce_get_value(const enum piece_type pt) {
    assert(validate_piece_type(pt));

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
* @param pt         The piece_type
* @return           uint8_t The array index
*/
uint8_t pce_get_array_idx(const enum piece_type pt) {
    assert(validate_piece_type(pt));

    return (uint8_t)pt;
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
 * @brief       Gets the piece lable for the given piece (eg, 'P' for white pawn, 'k' for black king)
 *
 * @param pce The piece
 * @return A character representing the labe
 */
char pce_get_label(const struct piece pce) {
    assert(validate_piece(pce));

    enum colour col = extract_colour(pce);
    enum piece_type pt = extract_piece_type(pce);

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
struct piece pce_get_from_label(const char c) {
    assert(validate_label(c));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

    enum colour col;
    enum piece_type pt;

    if (islower(c)) {
        col = BLACK;
    } else {
        col = WHITE;
    }

    char cu = (char)toupper(c);

    switch (cu) {
    case 'P':
        pt = PAWN;
        break;
    case 'R':
        pt = ROOK;
        break;
    case 'N':
        pt = KNIGHT;
        break;
    case 'B':
        pt = BISHOP;
        break;
    case 'Q':
        pt = QUEEN;
        break;
    case 'K':
        pt = KING;
        break;
    default:
        assert(false);
    }
#pragma GCC diagnostic pop

    return pce_create(pt, col);
}

/**
 * @brief       Validates a piece is within expected range of values
 *
 * @param pce The piece
 */
bool validate_piece(const struct piece pce) {
    const enum piece_type pt = extract_piece_type(pce);
    const enum colour col = extract_colour(pce);

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
bool validate_piece_type(const enum piece_type pt) {

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
}

static enum colour extract_colour(const struct piece pce) {
    return (enum colour)((pce.pce_val & COLOUR_MASK) >> COLOUR_SHIFT);
}

static enum piece_type extract_piece_type(const struct piece pce) {
    return (enum piece_type)(pce.pce_val & ~COLOUR_MASK);
}

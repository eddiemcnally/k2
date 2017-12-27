// Copyright (c) 2017 Eddie McNally
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "piece.h"

/**
 * Piece values
 */
enum piece_values
{
    PCE_VAL_PAWN    = 100,
    PCE_VAL_BISHOP  = 325,
    PCE_VAL_KNIGHT  = 325,
    PCE_VAL_ROOK    = 500,
    PCE_VAL_QUEEN   = 1000,
    PCE_VAL_KING    = 50000
};

#define     COLOUR_MASK     0x03


/**
 * @brief       Tests if piece is WHITE
 * @details     Tests if the given piece is WHITE
 *
 * @param piece The piece
 * @return true if WHITE, false otherwise
 */
bool pce_is_white(const enum piece pce)
{
    validate_piece(pce);
    return ((pce & WHITE) != 0);
}

/**
 * @brief       Tests if piece is BLACK
 * @details     Tests if the given piece is BLACK
 *
 * @param piece The piece
 * @return true if BLACK, false otherwise
 */
bool pce_is_black(const enum piece pce)
{
    validate_piece(pce);
    return ((pce & BLACK) != 0);
}

/**
 * @brief       Swaps the colour
 * @details     Takes a given colour/side, and returns the other colour/side
 *
 * @param colour The given colour
 * @return The opposite colour
 */
enum colour swap_side(const enum colour col)
{
    validate_colour(col);

    if (col == WHITE)
    {
        return BLACK;
    }
    return WHITE;
}

/**
 * @brief       Gets the colour of a piece
 * @details     Gets the colour associated with the given piece
 *
 * @param piece The piece
 * @return The colour of the given piece
 */
enum colour pce_get_colour(const enum piece pce)
{
    validate_piece(pce);

    if (pce_is_white(pce))
    {
        return WHITE;
    }
    return BLACK;
}

/**
 * @brief       Gets the value for the piece
 * @details     Gets the piece value of the given piece
 *
 * @param piece The piece
 * @return The piece value
 */
uint32_t pce_get_value(const enum piece pce)
{
    validate_piece(pce);

    enum piece p = pce_get_piece_type(pce);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

    switch (p)
    {
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
#pragma GCC diagnostic pop

}

/**
 * @brief       Gets the piece label
 * @details     Gets the piece lable for the given piece (eg, 'P' for white pawn, 'k' for black king)
 *
 * @param piece The piece
 * @return A character representing the labe
 */
char pce_get_label(const enum piece pce)
{
    validate_piece(pce);


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

    switch (pce)
    {
    case WPAWN:
        return 'P';
    case BPAWN:
        return 'p';
    case WBISHOP:
        return 'B';
    case BBISHOP:
        return 'b';
    case WKNIGHT:
        return 'N';
    case BKNIGHT:
        return 'n';
    case WROOK:
        return 'R';
    case BROOK:
        return 'r';
    case WQUEEN:
        return 'Q';
    case BQUEEN:
        return 'q';
    case WKING:
        return 'K';
    case BKING:
        return 'k';
    default:
        assert(false);
        return '-';
    }
#pragma GCC diagnostic pop

}

/**
 * @brief       Gets the piece given the piece label
 * @details     For the given piece label, returns the associated enum piece
 *
 * @param c The piece label
 * @return The associated piece
 */
enum piece pce_get_from_label(const char c)
{
    validate_label(c);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

    switch (c)
    {
    case 'p':
        return BPAWN;
    case 'r':
        return BROOK;
    case 'n':
        return BKNIGHT;
    case 'b':
        return BBISHOP;
    case 'q':
        return BQUEEN;
    case 'k':
        return BKING;
    case 'P':
        return WPAWN;
    case 'R':
        return WROOK;
    case 'N':
        return WKNIGHT;
    case 'B':
        return WBISHOP;
    case 'Q':
        return WQUEEN;
    case 'K':
        return WKING;
    default:
        assert(false);
    }
#pragma GCC diagnostic pop

}

/**
 * @brief       Gets the piece type
 * @details     Given a piece (eg, white pawn), returns the piece type (eg, pawn)
 *
 * @param piece The piece
 * @return The piece type
 */
enum piece pce_get_piece_type(const enum piece piece)
{
    return (enum piece)(piece & (uint8_t) (~COLOUR_MASK));
}

/**
 * @brief       Validates a piece
 * @details     Validates a piece is within expected range of values
 *
 * @param piece The piece
 */
void validate_piece(const enum piece pce)
{
#ifdef ENABLE_ASSERTS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

    switch (pce)
    {
    case WPAWN:
    case BPAWN:
    case WKNIGHT:
    case BKNIGHT:
    case WBISHOP:
    case BBISHOP:
    case WROOK:
    case BROOK:
    case WQUEEN:
    case BQUEEN:
    case WKING:
    case BKING:
        assert(true);
        break;
    default:
        assert(false);
        break;
    }
#pragma GCC diagnostic pop
#endif
}

/**
 * @brief       Validates a colour
 * @details     Validates a colour is within expected range of values
 *
 * @param col The colour
 */
void validate_colour(const enum colour col)
{
#ifdef ENABLE_ASSERTS
    assert(col == WHITE || col == BLACK);
#endif

}

/**
 * @brief       Validates the piece label
 * @details     Validates a piece label is within a valid range of values
 *
 * @param c The piece label
 */
void validate_label(const char c)
{
#ifdef ENABLE_ASSERTS
    switch (c)
    {
    case 'p':
    case 'r':
    case 'n':
    case 'b':
    case 'q':
    case 'k':
    case 'P':
    case 'R':
    case 'N':
    case 'B':
    case 'Q':
    case 'K':
        assert(true);
        break;
    default:
        printf("Invalid label %c\n", c);
        assert(false);
    }
#endif
}

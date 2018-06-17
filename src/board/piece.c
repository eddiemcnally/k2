/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "piece.h"

/**
 * Piece values
 */
enum piece_values {
        PCE_VAL_PAWN    = 100,
        PCE_VAL_BISHOP  = 325,
        PCE_VAL_KNIGHT  = 325,
        PCE_VAL_ROOK    = 500,
        PCE_VAL_QUEEN   = 1000,
        PCE_VAL_KING    = 50000
};

#define     COLOUR_MASK     0x03


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
bool pce_is_white ( const enum piece pce )
{
        assert ( validate_piece ( pce ) );
        return ( ( pce & WHITE ) != 0 );
}

/**
 * @brief       Tests if the given piece is BLACK
 *
 * @param pce   The piece
 * @return true if BLACK, false otherwise
 */
bool pce_is_black ( const enum piece pce )
{
        assert ( validate_piece ( pce ) );
        return ( ( pce & BLACK ) != 0 );
}

/**
 * @brief       Takes a given colour/side, and returns the other colour/side
 *
 * @param col The given colour
 * @return The opposite colour
 */
enum colour swap_side ( const enum colour col )
{
        assert ( validate_colour ( col ) );

        enum colour inverted = ~col;
        return ( enum colour ) ( inverted & COLOUR_MASK );
}

/**
 * @brief       Gets the colour associated with the given piece
 *
 * @param pce The piece
 * @return The colour of the given piece
 */
enum colour pce_get_colour ( const enum piece pce )
{
        assert ( validate_piece ( pce ) );
        return ( enum colour ) ( pce & COLOUR_MASK );
}


/**
* @brief Returns an enum value that can be used to denote "no piece"
*
* @return piece A "no piece" enum
*/
enum piece pce_get_no_piece ( void )
{
        return ( enum piece ) ( 0 );
}

/**
 * @brief       Gets the piece value of the given piece
 *
 * @param pce The piece
 * @return The piece value
 */
uint32_t pce_get_value ( const enum piece pce )
{
        assert ( validate_piece ( pce ) );
        enum piece_class p = pce_get_piece_class ( pce );

        switch ( p ) {
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
                assert ( false );
                break;
        }
}


/**
* @brief Converts the piece to an array index for use by various modules
*
* @param pce p_pce The piece
* @return uint8_t The array index
*/
uint8_t pce_get_array_idx ( const enum piece pce )
{
        assert ( validate_piece ( pce ) );
        switch ( pce ) {
        case WPAWN:
                return 0;
        case BPAWN:
                return 1;
        case WKNIGHT:
                return 2;
        case BKNIGHT:
                return 3;
        case WBISHOP:
                return 4;
        case BBISHOP:
                return 5;
        case WROOK:
                return 6;
        case BROOK:
                return 7;
        case WQUEEN:
                return 8;
        case BQUEEN:
                return 9;
        case WKING:
                return 10;
        case BKING:
                return 11;
        default:
                assert ( false );
        }
}


/**
* @brief Converts ths given colout to an array index.
*
* @param col p_col The colour
* @return uint8_t The array index
*/
uint8_t pce_col_get_array_idx ( const enum colour col )
{
        assert ( validate_colour ( col ) );
        switch ( col ) {
        case WHITE:
                return 0;
        case BLACK:
                return 1;
        default:
                assert ( false );
        }
}


/**
 * @brief       Gets the piece lable for the given piece (eg, 'P' for white pawn, 'k' for black king)
 *
 * @param pce The piece
 * @return A character representing the labe
 */
char pce_get_label ( const enum piece pce )
{
        assert ( validate_piece ( pce ) );

        switch ( pce ) {
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
                assert ( false );
                return '-';
        }
}

/**
 * @brief       For the given piece label, returns the associated enum piece
 *
 * @param c The piece label
 * @return The associated piece
 */
enum piece pce_get_from_label ( const char c )
{
        assert ( validate_label ( c ) );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

        switch ( c ) {
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
                assert ( false );
        }
#pragma GCC diagnostic pop

}

/**
 * @brief       Given a piece (eg, white pawn), returns the piece type (eg, pawn)
 *
 * @param piece The piece
 * @return The piece type
 */
enum piece_class pce_get_piece_class ( const enum piece piece )
{
        assert ( validate_piece ( piece ) );
        return ( enum piece_class ) ( piece & ( uint8_t ) ( ~COLOUR_MASK ) );
}



enum piece pce_get_piece ( const enum piece_class pc, const enum colour col )
{
        assert ( validate_piece_class ( pc ) );
        assert ( validate_colour ( col ) );

        return ( enum piece ) ( pc | col );
}




/**
 * @brief       Validates a piece is within expected range of values
 *
 * @param pce The piece
 */
bool validate_piece ( const enum piece pce )
{
        switch ( pce ) {
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
                return true;
        default:
                assert ( false );
        }
}

/**
 * @brief       Validates a piece class
 *
 * @param pce The piece class
 */
bool validate_piece_class ( const enum piece_class pc )
{
        switch ( pc ) {
        case PAWN:
        case KNIGHT:
        case BISHOP:
        case ROOK:
        case QUEEN:
        case KING:
                return true;
        default:
                assert ( false );
        }
}


/**
 * @brief       Validates a colour is within expected range of values
 *
 * @param col The colour
 */
bool validate_colour ( const enum colour col )
{
        switch ( col ) {
        case WHITE:
        case BLACK:
                return true;
        default:
                assert ( false );
        }
}

/**
 * @brief       Validates a piece label is within a valid range of values
 *
 * @param c The piece label
 */
bool validate_label ( const char c )
{
        switch ( c ) {
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
                return true;
        default:
                printf ( "Invalid label %c\n", c );
                assert ( false );
        }
}

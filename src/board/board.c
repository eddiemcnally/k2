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


#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "bitboard.h"
#include "piece.h"
#include "square.h"
#include "board.h"


/**
 * used for indexing into an array
 */
enum piece_offset {
        Pawn    = 0,
        Bishop  = 1,
        Knight  = 2,
        Rook    = 3,
        Queen   = 4,
        King    = 5
};

enum colour_offset {
        White,
        Black
};


/**
 * @brief       Represents the state of the board (squares, pieces)
 */
struct board {
        uint16_t    struct_init_key;

        // a set bit represents an occupied square
        bitboard_t  bb_board;

        // a bitboard per colour, a set bit means that colour occupies that square
        bitboard_t  bb_colour[NUM_COLOURS];

        // total material value for each colour
        uint32_t    material[NUM_COLOURS];

        // contains the piece on a given square
        enum piece  pce_square[NUM_SQUARES];

        // bitboard for each piece
        bitboard_t piece_bb[NUM_COLOURS][NUM_PIECE_TYPES];
};

#define     NO_PIECE    (NUM_PIECES + 1)

// used to check struct is populated when passed into public functions
#define STRUCT_INIT_KEY ((uint16_t)0xdeadbeef)

static void setup_square ( struct board *brd, const enum piece pce, const enum square sq );
static void clear_square ( struct board *brd, const enum piece pce, const enum square sq );
static bool validate_struct_init ( const struct board *brd );
static uint8_t map_piece_to_offset ( const enum piece pce );
static uint8_t map_colour_to_offset ( const enum colour col );
static void add_material ( struct board *brd, enum piece pce );
static void remove_material ( struct board *brd, enum piece pce );
static uint8_t get_colour_offset ( enum piece pce );

// ==================================================================
//
// public functions
//
// ==================================================================


/**
 * @brief       Allocates and initialises a board struct
 * @return      ptr to struct board
 */
struct board* brd_allocate ( void )
{
        struct board *retval = ( struct board * ) malloc ( sizeof ( struct board ) );

        memset ( retval, 0, sizeof ( struct board ) );
        retval->struct_init_key = STRUCT_INIT_KEY;

        for ( enum square sq = a1; sq <= h8; sq++ ) {
                retval->pce_square[sq] = NO_PIECE;
        }

        return retval;
}

/**
 * @brief       Clears and Deallocates the memory associated with a board
 *
 * @param brd   Pointer to the struct memory
 */
void brd_deallocate ( struct board* brd )
{
        assert ( validate_struct_init ( brd ) );

        memset ( brd, 0, sizeof ( struct board ) );
        free ( brd );
}



/**
 * @brief       Gets a bitboard for the entire board
 *
 * @param brd The board
 * @return A bitboard with a bit set for each occupier square
 */
bitboard_t brd_get_board_bb ( const struct board* brd )
{
        assert ( validate_struct_init ( brd ) );
        return brd->bb_board;
}

/**
 * @brief        Tests if given square is set on board
 * @param   brd  the board
 * @param   sq   the square to test
 * @return  true if square occupied, false otherwise
 */
bool brd_is_sq_occupied ( const struct board* brd, const enum square sq )
{
        assert ( validate_struct_init ( brd ) );

        return bb_is_set ( brd->bb_board, sq );
}



/**
 * @brief       Tries to return the piece on the given square
 * @param brd   the board
 * @param sq    the square
 * @param pce   the returned piece (if true)
 * @return true is piece found, false otherwise
 */
bool brd_try_get_piece_on_square ( const struct board* brd, const enum square sq, enum piece *pce )
{
        assert ( validate_struct_init ( brd ) );

        *pce = brd->pce_square[sq];
        return *pce != NO_PIECE;
}


/**
 * @brief       Add a piece to the board on the specified square
 *
 * @param brd   The board
 * @param pce   The piece to add
 * @param sq    The square
 */
void brd_add_piece ( struct board* brd, const enum piece pce, const enum square sq )
{
        assert ( validate_struct_init ( brd ) );

        setup_square ( brd, pce, sq );
        add_material ( brd, pce );
}

/**
 * @brief       Removes a piece from the specified square
 *
 * @param brd   The board
 * @param pce   The piece
 * @param sq    The square
 */
void brd_remove_piece ( struct board* brd, const enum piece pce, const enum square sq )
{
        assert ( validate_struct_init ( brd ) );

        clear_square ( brd, pce, sq );
        remove_material ( brd, pce );
}

/**
 * @brief       Move a piece from the "From" square to the "To" square
 *
 * @param brd   The board
 * @param pce   The piece to move
 * @param from_sq The From square
 * @param to_sq The To square
 */
void brd_move_piece ( struct board* brd, const enum piece pce, const enum square from_sq, const enum square to_sq )
{
        assert ( validate_struct_init ( brd ) );

        clear_square ( brd, pce, from_sq );
        setup_square ( brd, pce, to_sq );
}


/**
 * @brief       Gets the bitboard representing all pieces of the given colour
 *
 * @param brd   The board
 * @param colour The colour
 *
 * @return A bitboard with a bit set for each piece of the given colour
 */
bitboard_t brd_get_colour_bb ( const struct board* brd, const enum colour colour )
{
        assert ( validate_colour ( colour ) );

        uint8_t offset = map_colour_to_offset ( colour );
        return brd->bb_colour[offset];

}

/**
 * @brief       Returns a bitboard for the given piece, representing the location of all pieces of the given type.
 *
 * @param brd The board
 * @param pce The piece
 *
 * @return A bitboard for that piece
 */
bitboard_t brd_get_piece_bb ( const struct board* brd, const enum piece pce )
{
        assert ( validate_piece ( pce ) );

        enum colour col = pce_get_colour ( pce );
        uint8_t col_off = map_colour_to_offset ( col );
        uint8_t pce_off = map_piece_to_offset ( pce );

        return brd->piece_bb[col_off][pce_off];
}


/**
 * @brief       Validates that the internal state representation of the board is consistent
 *
 * @param brd The board to validate
 */
bool validate_board ( const struct board* brd )
{
        if ( brd->struct_init_key != STRUCT_INIT_KEY ) {
                return false;
        }
        return true;
        // TODO - expand

}



// ==================================================================
//
// private functions
//
// ==================================================================


static void add_material ( struct board *brd, enum piece pce )
{
        uint32_t material = pce_get_value ( pce );
        uint8_t offset = get_colour_offset ( pce );
        brd->material[offset] += material;
}

static void remove_material ( struct board *brd, enum piece pce )
{
        uint32_t material = pce_get_value ( pce );
        uint8_t offset = get_colour_offset ( pce );
        brd->material[offset] -= material;
}

static uint8_t get_colour_offset ( enum piece pce )
{
        enum colour col = pce_get_colour ( pce );
        return map_colour_to_offset ( col );
}


static void setup_square ( struct board *brd, const enum piece pce, const enum square sq )
{
        enum colour col = pce_get_colour ( pce );
        uint8_t pce_off = map_piece_to_offset ( pce );
        uint8_t col_off = map_colour_to_offset ( col );

        bb_set_square ( &brd->piece_bb[col_off][pce_off], sq );
        bb_set_square ( &brd->bb_board, sq );
        bb_set_square ( &brd->bb_colour[col_off], sq );
        brd->pce_square[sq] = pce;
}

static void clear_square ( struct board *brd, const enum piece pce, const enum square sq )
{
        enum colour col = pce_get_colour ( pce );
        uint8_t pce_off = map_piece_to_offset ( pce );
        uint8_t col_off = map_colour_to_offset ( col );

        bb_clear_square ( &brd->piece_bb[col_off][pce_off], sq );
        bb_clear_square ( &brd->bb_board, sq );
        bb_clear_square ( &brd->bb_colour[col_off], sq );
        brd->pce_square[sq] = NO_PIECE;
}

static uint8_t map_piece_to_offset ( const enum piece pce )
{
        enum piece pce_type = pce_get_piece_type ( pce );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

        switch ( pce_type ) {
        case PAWN:
                return Pawn;
        case BISHOP:
                return Bishop;
        case KNIGHT:
                return Knight;
        case ROOK:
                return Rook;
        case QUEEN:
                return Queen;
        case KING:
                return King;
        default:
                assert ( false );
        }
#pragma GCC diagnostic pop

}

static uint8_t map_colour_to_offset ( const enum colour col )
{
        switch ( col ) {
        case WHITE:
                return White;
        case BLACK:
                return Black;
        default:
                assert ( false );
        }
}



static bool validate_struct_init ( const struct board *brd )
{
        if ( brd->struct_init_key != STRUCT_INIT_KEY ) {
                return false;
        }
        return true;
}




// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

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
#include <stdio.h>
#include <string.h>
#include "bitboard.h"
#include "piece.h"
#include "square.h"
#include "board.h"


/**
 * @brief       Represents the state of the board (squares, pieces)
 */
struct board {
        uint32_t        struct_init_key;

        // a set bit represents an occupied square
        uint64_t        bb_board;

        // a bitboard per colour, a set bit means that colour occupies that square
        uint64_t        bb_colour[NUM_COLOURS];

        // total material value for each colour
        uint32_t        material[NUM_COLOURS];

        // contains the piece on a given square
        enum piece      pce_square[NUM_SQUARES];

        // bitboard for each piece
        uint64_t        piece_bb[NUM_PIECES];
};


// square operations
enum sq_op {
        SET_SQ,
        CLEAR_SQ
};


// used to check struct is populated when passed into public functions
static const uint32_t STRUCT_INIT_KEY = 0xdeadbeef;

static bool validate_struct_init ( const struct board *brd );
static bool validate_square_empty ( const struct board *brd, const enum square to_sq );
static bool validate_pce_on_sq ( const struct board *brd, const enum piece pce, enum square sq );
static void add_material ( struct board *brd, enum piece pce );
static void remove_material ( struct board *brd, enum piece pce );
static void init_struct ( struct board *brd );
static void populate_square ( struct board *brd, const enum piece pce, const enum square sq, const enum sq_op operation );


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
        init_struct ( retval );

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
uint64_t brd_get_board_bb ( const struct board* brd )
{
        assert ( validate_board ( brd ) );
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
        assert ( validate_board ( brd ) );
        assert ( validate_square ( sq ) );

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
        assert ( validate_board ( brd ) );
        assert ( validate_square ( sq ) );

        *pce = brd->pce_square[sq];
        return *pce != pce_get_no_piece();
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
        assert ( validate_board ( brd ) );
        assert ( validate_square ( sq ) );
        assert ( validate_piece ( pce ) );
        assert ( validate_square_empty ( brd, sq ) );

        populate_square ( brd, pce, sq, SET_SQ );
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
        assert ( validate_board ( brd ) );
        assert ( validate_square ( sq ) );
        assert ( validate_piece ( pce ) );
        assert ( validate_pce_on_sq ( brd, pce, sq ) );

        populate_square ( brd, pce, sq, CLEAR_SQ );
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
        assert ( validate_board ( brd ) );
        assert ( validate_square ( from_sq ) );
        assert ( validate_square ( to_sq ) );
        assert ( validate_piece ( pce ) );
        assert ( validate_square_empty ( brd, to_sq ) );
        assert ( validate_pce_on_sq ( brd, pce, from_sq ) );

        populate_square ( brd, pce, from_sq, CLEAR_SQ );
        populate_square ( brd, pce, to_sq, SET_SQ );
}


/**
 * @brief       Gets the bitboard representing all pieces of the given colour
 *
 * @param brd   The board
 * @param colour The colour
 *
 * @return A bitboard with a bit set for each piece of the given colour
 */
uint64_t brd_get_colour_bb ( const struct board* brd, const enum colour colour )
{
        assert ( validate_colour ( colour ) );
        assert ( validate_board ( brd ) );

        uint8_t offset = pce_col_get_array_idx ( colour );
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
uint64_t brd_get_piece_bb ( const struct board* brd, const enum piece pce )
{
        assert ( validate_piece ( pce ) );
        assert ( validate_board ( brd ) );

        uint8_t pce_off = pce_get_array_idx ( pce );

        return brd->piece_bb[pce_off];
}


/**
 * @brief       Validates that the internal state representation of the board is consistent
 *
 * @param brd The board to validate
 */
bool validate_board ( const struct board* brd )
{
        enum square sq;
        bool is_occupied;

        if ( brd->struct_init_key != STRUCT_INIT_KEY ) {
                assert ( false );
        }

        // check various bitboards agree with the pices on the squares
        for ( sq = a1; sq <= h8; sq++ ) {
                is_occupied = bb_is_set ( brd->bb_board, sq );

                // conflate colour bitboards
                const uint8_t white_idx = pce_col_get_array_idx ( WHITE );
                const uint8_t black_idx = pce_col_get_array_idx ( BLACK );
                const uint64_t white_bb = brd->bb_colour[white_idx];
                const uint64_t black_bb = brd->bb_colour[black_idx];
                const uint64_t conflated_col_bb = white_bb | black_bb;

                if ( is_occupied ) {
                        assert ( bb_is_set ( conflated_col_bb, sq ) );
                        assert ( brd->pce_square[sq] != pce_get_no_piece() );
                        assert ( bb_is_set ( brd->bb_board, sq ) );
                } else {
                        assert ( bb_is_clear ( conflated_col_bb, sq ) );
                        assert ( brd->pce_square[sq] == pce_get_no_piece() );
                        assert ( bb_is_clear ( brd->bb_board, sq ) );
                }
        }

        // conflate all piece bitboards, and verify same as board bitboard
        uint64_t conflated_pce_bb = 0;
        for ( int i = 0; i < NUM_PIECES; i++ ) {
                conflated_pce_bb |= brd->piece_bb[i];
        }
        assert ( conflated_pce_bb == brd->bb_board );

        return true;
}



// ==================================================================
//
// private functions
//
// ==================================================================


static void init_struct ( struct board *brd )
{
        memset ( brd, 0, sizeof ( struct board ) );

        brd->bb_board = bb_get_empty();
        for ( int i = 0; i < NUM_COLOURS; i++ ) {
                brd->bb_colour[i] = bb_get_empty();
                brd->material[i] = 0;
        }

        for ( enum square sq = a1; sq <= h8; sq++ ) {
                brd->pce_square[sq] = pce_get_no_piece();
        }

        brd->struct_init_key = STRUCT_INIT_KEY;
}



static void add_material ( struct board *brd, enum piece pce )
{
        const uint32_t material = pce_get_value ( pce );
        const enum colour col = pce_get_colour ( pce );
        const uint8_t offset = pce_col_get_array_idx ( col );
        brd->material[offset] += material;
}

static void remove_material ( struct board *brd, enum piece pce )
{
        const uint32_t material = pce_get_value ( pce );
        const enum colour col = pce_get_colour ( pce );
        const uint8_t offset = pce_col_get_array_idx ( col );
        brd->material[offset] -= material;
}



static void populate_square ( struct board *brd, const enum piece pce,
                              const enum square sq, const enum sq_op operation )
{
        const enum colour col = pce_get_colour ( pce );
        const uint8_t pce_off = pce_get_array_idx ( pce );
        const uint8_t col_off = pce_col_get_array_idx ( col );

        uint64_t pce_bb = brd->piece_bb[pce_off];
        uint64_t brd_bb = brd->bb_board;
        uint64_t col_bb = brd->bb_colour[col_off];

        switch ( operation ) {
        case CLEAR_SQ:
                pce_bb = bb_clear_square ( pce_bb, sq );
                brd_bb = bb_clear_square ( brd_bb, sq );
                col_bb = bb_clear_square ( col_bb, sq );
                brd->pce_square[sq] = pce_get_no_piece();
                break;
        case SET_SQ:
                pce_bb = bb_set_square ( pce_bb, sq );
                brd_bb = bb_set_square ( brd_bb, sq );
                col_bb = bb_set_square ( col_bb, sq );
                brd->pce_square[sq] = pce;
                break;
        default:
                assert ( false );
        }

        brd->piece_bb[pce_off] = pce_bb;
        brd->bb_board = brd_bb;
        brd->bb_colour[col_off] = col_bb;
}




static bool validate_struct_init ( const struct board *brd )
{
        if ( brd->struct_init_key != STRUCT_INIT_KEY ) {
                return false;
        }
        return true;
}


static bool validate_square_empty ( const struct board *brd, const enum square sq )
{
        uint64_t bb = brd_get_board_bb ( brd );
        bool is_set = bb_is_set ( bb, sq );
        return is_set == false;
}


static bool validate_pce_on_sq ( const struct board *brd, const enum piece pce, enum square sq )
{
        enum piece pce_on_brd;
        bool found = brd_try_get_piece_on_square ( brd, sq, &pce_on_brd );
        if ( found == false ) {
                return false;
        }
        if ( pce_on_brd != pce ) {
                return false;
        }
        return true;
}

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
 * @brief       The main Board struct
 * @details     Represents the state of the board (squares, pieces)
 *
 */
struct board
{
    uint16_t    struct_init_key;

    // a set bit represents an occupied square
    bitboard_t  bb_board;

    // a bitboard per colour, a set bit means that colour occupies that square
    bitboard_t  bb_colour[NUM_COLOURS];

    // total material value for each colour
    uint32_t    material[NUM_COLOURS];

    // contains the piece on a given square
    enum piece  pce_square[NUM_SQUARES];
};

#define     NO_PIECE    0

// used to check struct is populated when passed into public functions
#define STRUCT_INIT_KEY ((uint16_t)0xdeadbeef)

static void setup_square(struct board *brd, const enum piece pce, const enum square sq, const enum colour col);
static void clear_square(struct board *brd, const enum square sq, const enum colour col);
static void validate_struct_init(const struct board *brd);

// ==================================================================
//
// public functions
//
// ==================================================================


/**
 * @brief       Allocates and initialises a board struct
 * @return      ptr to struct board
 */
struct board* brd_allocate(void)
{
    struct board *retval = (struct board *)malloc(sizeof(struct board));

    memset(retval, 0, sizeof(struct board));
    retval->struct_init_key = STRUCT_INIT_KEY;

    return retval;
}

/**
 * @brief       Deallocates the memory associated with a board
 * @details     Clears memory prior to deallocating
 *
 * @param board Pointer to the struct memory
 */
void brd_deallocate(struct board* brd)
{
    validate_struct_init(brd);

    memset(brd, 0, sizeof (struct board));
    free(brd);
}



/**
 * @brief        Tests if given square is set on board
 * @param   brd  the board
 * @param   sq   the square to test
 * @return  true if square occupied, false otherwise
 */
bool brd_is_sq_occupied(const struct board* brd, const enum square sq)
{
    validate_struct_init(brd);

    return bb_is_set(brd->bb_board, sq);
}



/**
 * @brief       Tries to return the piece on the given square
 * @param brd   the board
 * @param sq    the square
 * @param pce   the returned piece (if true)
 * @return true is piece found, false otherwise
 */
bool brd_try_get_piece_on_square(const struct board* brd, const enum square sq, enum piece *pce)
{
    validate_struct_init(brd);

    *pce = brd->pce_square[sq];
    return *pce != NO_PIECE;
}


/**
 * @brief       Add piece to board
 * @details     Add a piece to the board on the specified square
 *
 * @param board     The board
 * @param piece     The piece to add
 * @param square    The square
 */
void brd_add_piece(struct board* brd, const enum piece pce, const enum square sq)
{
    validate_struct_init(brd);

    enum colour col = pce_get_colour(pce);
    setup_square(brd, pce, sq, col);

    uint32_t material = pce_get_value(pce);
    brd->material[col] += material;
}

/**
 * @brief       Remove a piece from the board
 * @details     Removes a piece from the specified square
 *
 * @param board     The board
 * @param piece     The piece
 * @param square    The square
 */
void brd_remove_piece(struct board* brd, const enum piece pce, const enum square sq)
{
    validate_struct_init(brd);

    enum colour col = pce_get_colour(pce);
    clear_square(brd, sq, col);

    uint32_t material = pce_get_value(pce);
    brd->material[col] -= material;
}

/**
 * @brief       Move a piece
 * @details     Move a piece from the "From" square to the "To" square
 *
 * @param board     The board
 * @param piece     The piece to move
 * @param square    The From square
 * @param square    The To square
 */
void brd_move_piece(struct board* brd, const enum piece pce, const enum square from_sq, const enum square to_sq)
{
    validate_struct_init(brd);

    enum colour col = pce_get_colour(pce);

    clear_square(brd, from_sq, col);
    setup_square(brd, pce, to_sq, col);
}



// ==================================================================
//
// private functions
//
// ==================================================================


static void setup_square(struct board *brd, const enum piece pce, const enum square sq, const enum colour col)
{
    bb_set_square(&brd->bb_board, sq);
    bb_set_square(&brd->bb_colour[col], sq);
    brd->pce_square[sq] = pce;
}

static void clear_square(struct board *brd, const enum square sq, const enum colour col)
{
    bb_clear_square(&brd->bb_board, sq);
    bb_clear_square(&brd->bb_colour[col], sq);
    brd->pce_square[sq] = NO_PIECE;
}

static void validate_struct_init(const struct board *brd)
{
    if (brd->struct_init_key != STRUCT_INIT_KEY)
    {
        assert(false);
    }
}

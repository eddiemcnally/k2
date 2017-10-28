#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "bitboard.h"
#include "piece.h"
#include "square.h"
#include "board.h"

struct board {
    // a set bit represents an occupied square
    bitboard_t board;

    // a bitboard per colour, a set bit means that colour occupies that square
    bitboard_t colour_bb[NUM_COLOURS];

    // total material value for each colour
    uint32_t material[NUM_COLOURS];

    // contains the piece on a given square
    enum piece pce_square[NUM_SQUARES];
};

#define     NO_PIECE    0


static struct board the_board;

static void setup_square(struct board *brd, const enum piece pce, const enum square sq, const enum colour col);
static void clear_square(struct board *brd, const enum square sq, const enum colour col);


// ====================
// Public functions
// ====================

/**
 * @brief Initialises and returns ptr to a clean board
 * @return struct board *
 */
struct board* get_empty_board(void)
{
    memset(&the_board, 0, sizeof (struct board));
    return &the_board;
}


/**
 * @brief Tests if given square is set on board
 * @param brd the board
 * @param sq the square to test
 * @return true if square occupied, false otherwise
 */
bool is_sq_occupied(const struct board* brd, const enum square sq)
{
    return bb_is_set(brd->board, sq);
}



/**
 * @brief Tries to return the piece on the given square
 * @param brd the board
 * @param sq the square
 * @param pce the returned piece (if true)
 * @return true is piece found, false otherwise
 */
bool try_get_piece_on_square(const struct board* brd, const enum square sq, enum piece *pce)
{
    *pce = brd->pce_square[sq];
    if ((*pce != NO_PIECE)) {
        return true;
    }
    return false;
}


void add_piece(struct board* brd, const enum piece pce, const enum square sq)
{
    enum colour col = pce_get_colour(pce);
    setup_square(brd, pce, sq, col);

    uint32_t material = pce_get_value(pce);
    brd->material[col] += material;
}

void remove_piece(struct board* brd, const enum piece pce, const enum square sq)
{
    enum colour col = pce_get_colour(pce);
    clear_square(brd, sq, col);

    uint32_t material = pce_get_value(pce);
    brd->material[col] -= material;
}

void move_piece(struct board* brd, const enum piece pce, const enum square from_sq, const enum square to_sq)
{
    enum colour col = pce_get_colour(pce);

    clear_square(brd, from_sq, col);
    setup_square(brd, pce, to_sq, col);
}



// =======================
// Private functions
// =======================

static void setup_square(struct board *brd, const enum piece pce, const enum square sq, const enum colour col)
{
    bb_set_square(&brd->board, sq);
    bb_set_square(&brd->colour_bb[col], sq);
    brd->pce_square[sq] = pce;
}

static void clear_square(struct board *brd, const enum square sq, const enum colour col)
{
    bb_clear_square(&brd->board, sq);
    bb_clear_square(&brd->colour_bb[col], sq);
    brd->pce_square[sq] = NO_PIECE;
}

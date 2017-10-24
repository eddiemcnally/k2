#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "bitboard.h"
#include "piece.h"
#include "square.h"
#include "board.h" 

struct board {
    // bitboard entry for each piece, with a set
    // bit representing an occupied square for that piece type
    bitboard_t pieces[NUM_PIECES];

    // a set bit represents an occupied square
    bitboard_t board;

    // a bitboard per colour, a set bit means that colour occupies that square
    bitboard_t colour_bb[NUM_COLOURS];

    // total material value for each colour
    uint32_t material[NUM_COLOURS];

    // contains the piece on a given square
    enum piece pce_square[NUM_SQUARES];
};


static struct board the_board;

static void set_bitboard(struct board *brd, const enum piece pce, const enum square sq, const enum colour col);
static void clear_bitboard(struct board *brd, const enum piece pce, const enum square sq, const enum colour col);

struct board* get_empty_board(void)
{
    memset(&the_board, 0, sizeof (struct board));
    return &the_board;
}

bool is_sq_occupied(const struct board* brd, const enum square sq)
{
    return bb_is_set(brd->board, sq);
}

void add_piece(struct board* brd, const enum piece pce, const enum square sq)
{
    enum colour col = pce_get_colour(pce);
    set_bitboard(brd, pce, sq, col);

    uint32_t material = pce_get_value(pce);
    brd->material[col] += material;
}

void remove_piece(struct board* brd, const enum piece pce, const enum square sq)
{
    enum colour col = pce_get_colour(pce);
    clear_bitboard(brd, pce, sq, col);
    uint32_t material = pce_get_value(pce);
    brd->material[col] -= material;
}

void move_piece(struct board* brd, const enum piece pce, const enum square from_sq, const enum square to_sq)
{
    enum colour col = pce_get_colour(pce);

    clear_bitboard(brd, pce, from_sq, col);
    set_bitboard(brd, pce, to_sq, col);
}

static void set_bitboard(struct board *brd, const enum piece pce, const enum square sq, const enum colour col)
{
    bb_set_square(&brd->pieces[pce], sq);
    bb_set_square(&brd->board, sq);
    bb_set_square(&brd->colour_bb[col], sq);
}

static void clear_bitboard(struct board *brd, const enum piece pce, const enum square sq, const enum colour col)
{
    bb_clear_square(&brd->pieces[pce], sq);
    bb_clear_square(&brd->board, sq);
    bb_clear_square(&brd->colour_bb[col], sq);
}




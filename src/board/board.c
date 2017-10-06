#include <stdint.h>
#include <stdbool.h>
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

	uint32_t material[NUM_COLOURS];
};


static void validate_square(const enum square sq);
static void validate_piece(const enum piece pce);


bool is_sq_occupied(const struct board* brd)
{
	return true;

}

void add_piece(struct board* brd, const enum piece pce, const enum square sq)
{

}

void remove_piece(struct board* brd, const enum piece pce, const enum square sq)
{

}

void move_piece(struct board* brd, const enum piece pce, const enum square from_sq, const enum square to_sq)
{

}



static void validate_square(const enum square sq)
{
#ifdef ENABLE_ASSERTS
	assert(sq >= a1 && sq <= h8);
#endif
}

static void validate_piece(const enum piece pce)
{
#ifdef ENABLE_ASSERTS
	assert(pce >= WPAWN && pce <= BKING);
#endif
}



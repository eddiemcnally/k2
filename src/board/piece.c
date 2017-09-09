#include <stdbool.h>
#include <assert.h>
#include "piece.h"
#include "utils.h"


#define COLOUR_MASK 	0x01		// masks off the LSB, indicating colour
#define NUM_PIECES	12

/**
 * Contains the piece values
 */
static const uint32_t piece_values[NUM_PIECES] = {
	100,	// WPAWN
	100,	// BPAWN
	325,	// WBISHOP
	325,	// BBISHOP
	325,	// WKNIGHT
	325,	// BKNIGHT
	550,	// WROOK
	550,	// BROOK
	1000,	// WQUEEN
	1000,	// BQUEEN
	50000,	// WKING
	50000	// BKING
};


/**
 * Labels for printing out pieces
 */
static const char piece_labels[NUM_PIECES] = {'P', 'p', 'B', 'b', 'N', 'n', 'R', 'r', 'Q', 'q', 'K', 'k'};

static void validate_piece(const enum piece pce);
static void validate_colour(const enum colour col);


bool is_white(const enum piece pce)
{
	validate_piece(pce);

	if ( (pce & COLOUR_MASK) == 0) {
		return true;
	}
	return false;
}

bool is_black(const enum piece pce)
{
	validate_piece(pce);

	return is_white(pce) == false;
}

enum colour swap_side(const enum colour col)
{
	validate_colour(col);

	if (col == WHITE) {
		return BLACK;
	}
	return WHITE;
}

enum colour get_colour(const enum piece pce)
{
	validate_piece(pce);

	if (is_white(pce)) {
		return WHITE;
	}
	return BLACK;
}

uint32_t get_piece_value(const enum piece pce)
{
	validate_piece(pce);

	return	piece_values[pce];
}


char get_label(const enum piece pce)
{
	validate_piece(pce);

	return piece_labels[pce];
}

static inline void validate_piece(const enum piece pce)
{
#ifdef ENABLE_ASSERTS
	assert(pce >= WPAWN && pce <= BKING);
#endif
}

static void validate_colour(const enum colour col)
{
#ifdef ENABLE_ASSERTS
	assert(col == WHITE || col == BLACK);
#endif

}




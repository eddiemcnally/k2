#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "piece.h"


#define COLOUR_MASK 	0x01		// masks off the LSB, indicating colour



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
static void validate_label(const char c);


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

enum piece get_piece_from_label(const char c)
{
	validate_label(c);
	switch (c) {
	case 'p':
		return WPAWN;
	case 'r':
		return WROOK;
	case 'n':
		return WKNIGHT;
	case 'b':
		return WBISHOP;
	case 'q':
		return WQUEEN;
	case 'k':
		return WKING;
	case 'P':
		return BPAWN;
	case 'R':
		return BROOK;
	case 'N':
		return BKNIGHT;
	case 'B':
		return BBISHOP;
	case 'Q':
		return BQUEEN;
	case 'K':
		return BKING;
	default:
		assert(false);
	}
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

static void validate_label(const char c)
{
#ifdef ENABLE_ASSERTS
	switch (c) {
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
	default:
		assert(false);
	}
#endif
}


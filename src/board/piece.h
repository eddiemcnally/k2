#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "piece.h"


// Definition of pieces.
enum piece {
	WPAWN 		= 0,
	BPAWN 		= 1,
	WBISHOP 	= 2,
	BBISHOP 	= 3,
	WKNIGHT 	= 4,
	BKNIGHT 	= 5,
	WROOK 		= 6,
	BROOK 		= 7,
	WQUEEN 		= 8,
	BQUEEN 		= 9,
	WKING 		= 10,
	BKING 		= 11,
};

#define NUM_PIECES		12

enum colour {
	WHITE = 0,
	BLACK = 1
};

#define NUM_COLOURS		2



bool is_white(const enum piece pce);
bool is_black(const enum piece pce);
enum colour swap_side(const enum colour side);
enum colour get_colour(const enum piece pce);
uint32_t get_piece_value(const enum piece pce);
char get_label(const enum piece pce);
enum piece get_piece_from_label(const char c);


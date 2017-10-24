#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "bitboard.h"

void bb_set_square(bitboard_t* bb, const enum square sq)
{
	validate_square(sq);
	*bb = *bb | (0x01ull << sq);
}

void bb_clear_square(bitboard_t* bb, const enum square sq)
{
	validate_square(sq);
	*bb = *bb & (~(0x01ull << sq));
}

bool bb_is_set(bitboard_t bb, const enum square sq)
{
	return(bb & (0x01ull << sq)) != 0;
}


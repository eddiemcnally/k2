#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "bitboard.h"

static void validate_square(const enum square sq);


void set_bit(bitboard_t* bb, const enum square sq)
{
	validate_square(sq);
	*bb = *bb | (0x01ull << sq);
}

void clear_bit(bitboard_t* bb, const enum square sq)
{
	validate_square(sq);
	*bb = *bb & (~(0x01ull << sq));
}


static void validate_square(const enum square sq)
{
#ifdef ENABLE_ASSERTS
	assert(sq >= a1 && sq <= h8);
#endif
}

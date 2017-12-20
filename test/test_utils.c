#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include "bitboard.h"
#include "square.h"
#include "utils.h"
#include "test_utils.h"




void test_utils_pop_first_bit_all_squares(void **state)
{
	bitboard_t val = 0;

	for (enum square sq = a1; sq <= h8; sq++)
	{
		bb_set_square(&val, sq);
	}

	assert_true(val == 0xffffffffffffffff);

	for (enum square sq = a1; sq <= h8; sq++)
	{
		enum square popped = pop_1st_bit(&val);
		assert_true(popped == sq);
	}

	assert_true(val == 0);
}



void test_utils_pop_first_bit_random_squares(void **state)
{

	bitboard_t val = 0;

	bb_set_square(&val, a1);
	bb_set_square(&val, b5);
	bb_set_square(&val, c7);
	bb_set_square(&val, d3);
	bb_set_square(&val, e5);
	bb_set_square(&val, f8);
	bb_set_square(&val, g1);
	bb_set_square(&val, h8);

	enum square popped = pop_1st_bit(&val);
	assert_true(popped == a1);

	popped = pop_1st_bit(&val);
	assert_true(popped == g1);

	popped = pop_1st_bit(&val);
	assert_true(popped == d3);

	popped = pop_1st_bit(&val);
	assert_true(popped == b5);

	popped = pop_1st_bit(&val);
	assert_true(popped == e5);

	popped = pop_1st_bit(&val);
	assert_true(popped == c7);

	popped = pop_1st_bit(&val);
	assert_true(popped == f8);

	popped = pop_1st_bit(&val);
	assert_true(popped == h8);

	assert_true(val == 0);
}


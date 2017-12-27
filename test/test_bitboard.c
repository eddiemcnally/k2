/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */


#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdint.h>
#include "square.h"
#include "bitboard.h"
#include "test_bitboard.h"

void test_bitboard_pop_first_bit_all_squares(void **state)
{
	bitboard_t val = 0;
	for (enum square sq = a1; sq <= h8; sq++)
	{
		bb_set_square(&val, sq);
	}

	assert_true(val == 0xffffffffffffffff);
	for (enum square sq = a1; sq <= h8; sq++)
	{
		enum square popped = bb_pop_1st_bit(&val);
		assert_true(popped == sq);
	}
	assert_true(val == 0);
}



void test_bitboard_pop_first_bit_random_squares(void **state)
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

	enum square popped = bb_pop_1st_bit(&val);
	assert_true(popped == a1);

	popped = bb_pop_1st_bit(&val);
	assert_true(popped == g1);

	popped = bb_pop_1st_bit(&val);
	assert_true(popped == d3);

	popped = bb_pop_1st_bit(&val);
	assert_true(popped == b5);

	popped = bb_pop_1st_bit(&val);
	assert_true(popped == e5);

	popped = bb_pop_1st_bit(&val);
	assert_true(popped == c7);

	popped = bb_pop_1st_bit(&val);
	assert_true(popped == f8);

	popped = bb_pop_1st_bit(&val);
	assert_true(popped == h8);

	assert_true(val == 0);
}


void test_bitboard_count_bits(void **state)
{
	bitboard_t bb = (bitboard_t)0xFE100CA541;
	uint8_t num_bits_set = bb_count_bits(bb);
	assert_true(num_bits_set == 16);

	bb = (bitboard_t)0;
	num_bits_set = bb_count_bits(bb);
	assert_true(num_bits_set == 0);

	bb = (bitboard_t)1;
	num_bits_set = bb_count_bits(bb);
	assert_true(num_bits_set == 1);

	bb = (bitboard_t)0xFFFFFFFFFFFFFFFF;
	num_bits_set = bb_count_bits(bb);
	assert_true(num_bits_set == 64);
}



void test_bitboard_set_test_clear_bit(void **state)
{
	bitboard_t bb = (bitboard_t)0;
	bool is_set = false;

	for (enum square sq = a1; sq <= h8; sq++)
	{
		is_set = bb_is_set(bb, sq);
		assert_false(is_set);

		bb_set_square(&bb, sq);
		is_set = bb_is_set(bb, sq);
		assert_true(is_set);

		bb_clear_square(&bb, sq);
		is_set = bb_is_set(bb, sq);
		assert_false(is_set);
	}

	bb = (bitboard_t)0;
	for (enum square sq = a1; sq <= h8; sq++)
	{
		is_set = bb_is_set(bb, sq);
		assert_false(is_set);
	}

	bb = (bitboard_t)0xFFFFFFFFFFFFFFFF;
	for (enum square sq = a1; sq <= h8; sq++)
	{
		is_set = bb_is_set(bb, sq);
		assert_true(is_set);
	}
}


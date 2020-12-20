/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without 
 *  restriction, including without limitation the rights to use, 
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following 
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "test_bitboard.h"
#include "bitboard.h"
#include "square.h"
#include <cmocka.h>

void test_bitboard_pop_first_bit_all_squares(void **state) {
    uint64_t val = 0;
    for (enum square sq = a1; sq <= h8; sq++) {
        val = bb_set_square(val, sq);
    }

    assert_true(val == 0xffffffffffffffff);
    for (enum square sq = a1; sq <= h8; sq++) {
        enum square popped = bb_pop_1st_bit(val);
        val = bb_clear_square(val, popped);
        assert_true(popped == sq);
    }
    assert_true(val == 0);
}

void test_bitboard_pop_first_bit_random_squares(void **state) {

    uint64_t val = 0;

    val = bb_set_square(val, a1);
    val = bb_set_square(val, b5);
    val = bb_set_square(val, c7);
    val = bb_set_square(val, d3);
    val = bb_set_square(val, e5);
    val = bb_set_square(val, f8);
    val = bb_set_square(val, g1);
    val = bb_set_square(val, h8);

    enum square popped = bb_pop_1st_bit(val);
    val = bb_clear_square(val, popped);
    assert_true(popped == a1);

    popped = bb_pop_1st_bit(val);
    val = bb_clear_square(val, popped);
    assert_true(popped == g1);

    popped = bb_pop_1st_bit(val);
    val = bb_clear_square(val, popped);
    assert_true(popped == d3);

    popped = bb_pop_1st_bit(val);
    val = bb_clear_square(val, popped);
    assert_true(popped == b5);

    popped = bb_pop_1st_bit(val);
    val = bb_clear_square(val, popped);
    assert_true(popped == e5);

    popped = bb_pop_1st_bit(val);
    val = bb_clear_square(val, popped);
    assert_true(popped == c7);

    popped = bb_pop_1st_bit(val);
    val = bb_clear_square(val, popped);
    assert_true(popped == f8);

    popped = bb_pop_1st_bit(val);
    val = bb_clear_square(val, popped);
    assert_true(popped == h8);

    assert_true(val == 0);
}

void test_bitboard_reverse_bits(void **state) {
    uint64_t test_word = 0xFA340A7314DA;
    uint64_t reversed = bb_reverse(test_word);
    assert_true(reversed == 0x5B28CE502C5F0000);

    test_word = 0x6F43DA3E2;
    reversed = bb_reverse(test_word);
    assert_true(reversed == 0x47C5BC2F60000000);

    // swap the above to confirm
    test_word = 0x47C5BC2F60000000;
    reversed = bb_reverse(test_word);
    assert_true(reversed == 0x6F43DA3E2);

    test_word = 0x3AD1FDE008934A;
    reversed = bb_reverse(test_word);
    assert_true(reversed == 0x52C91007BF8B5C00);

    test_word = 0x00000000000000AA;
    reversed = bb_reverse(test_word);
    assert_true(reversed == 0x5500000000000000);
}

void test_bitboard_set_test_clear_bit(void **state) {
    uint64_t bb = (uint64_t)0;
    bool is_set = false;

    for (enum square sq = a1; sq <= h8; sq++) {
        is_set = bb_is_set(bb, sq);
        assert_false(is_set);

        bb = bb_set_square(bb, sq);
        is_set = bb_is_set(bb, sq);
        assert_true(is_set);

        bb = bb_clear_square(bb, sq);
        is_set = bb_is_set(bb, sq);
        assert_false(is_set);
    }

    bb = (uint64_t)0;
    for (enum square sq = a1; sq <= h8; sq++) {
        is_set = bb_is_set(bb, sq);
        assert_false(is_set);
    }

    bb = (uint64_t)0xFFFFFFFFFFFFFFFF;
    for (enum square sq = a1; sq <= h8; sq++) {
        is_set = bb_is_set(bb, sq);
        assert_true(is_set);
    }
}

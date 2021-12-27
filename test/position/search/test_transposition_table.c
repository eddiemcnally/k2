/*  MIT License
 *
 *  Copyright (c) 2020 Eddie McNally
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

#include "test_transposition_table.h"
#include "rand.h"
#include "transposition_table.h"

#include <cmocka.h>
#include <stdint.h>

#define MILLION 1000000
#define MB 1000000

void test_transposition_table_create_different_sizes_as_expected(void **state) {
#define NUM_TO_TEST_DIFF_SIZES 3
    uint32_t sizes[NUM_TO_TEST_DIFF_SIZES] = {100 * MB, 400 * MB, 53 * MB};

    for (int i = 0; i < NUM_TO_TEST_DIFF_SIZES; i++) {
        tt_create(sizes[i]);

        assert_true(tt_capacity() > 0);

        tt_dispose();
    }
}

void test_transposition_table_add_multiple_all_present(void **state) {

    // test a small number to avoid collisions

#define TABLE_SIZE (100 * MILLION)
#define NUM_TO_ADD 20000

    const uint64_t mv = move_encode_quiet(a1, b2);
    const int32_t score = 1234;
    const enum node_type nt = NODE_EXACT;
    const uint8_t depth = 5;

    tt_create((TABLE_SIZE * tt_entry_size()));

    // populate tt with test entries
    for (uint64_t i = 0; i < NUM_TO_ADD; i++) {
        uint64_t hash = i;
        const bool added = tt_add(hash, mv, depth, score, nt);
        assert_true(added);
    }

    // test all are in the table;
    for (uint64_t i = 0; i < NUM_TO_ADD; i++) {
        uint64_t found_mv;

        uint64_t h = i;

        const bool found = tt_probe_position(h, &found_mv);
        assert_true(found);
    }

    tt_dispose();
}

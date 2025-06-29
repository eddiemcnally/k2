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
#include "test_move_list.h"
#include "move_list.h"
#include <cmocka.h>

void test_move_list_init(void **state) {
    struct move_list mvl = mvl_initialise();

    uint16_t count = mvl_get_move_count(&mvl);
    assert_true(count == 0);
}

void test_move_list_bulk_add_moves(void **state) {
    const uint16_t max_moves = mvl_get_mvl_max_size() - 1;

    struct move_list mvl = mvl_initialise();

    // add moves
    for (int i = 0; i < max_moves; i++) {
        const struct move mv = move_encode_quiet(a1, b3);
        mvl_add(&mvl, mv);
    }

    uint16_t count = mvl_get_move_count(&mvl);
    assert_true(count == max_moves);
}

void test_move_list_reset_list(void **state) {
    const uint16_t num_moves = 30;
    struct move_list mvl = mvl_initialise();

    // add moves
    for (int i = 0; i < num_moves; i++) {
        struct move mv = move_encode_quiet(a1, a3);
        mvl_add(&mvl, mv);
    }

    assert_true(num_moves == mvl_get_move_count(&mvl));

    mvl_reset(&mvl);
    assert_true(0 == mvl_get_move_count(&mvl));
}

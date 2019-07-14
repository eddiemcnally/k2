/*  MIT License
 *
 *  Copyright (c) 2019 Eddie McNally
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

#include "test_hashkeys.h"
#include "hashkeys.h"
#include <cmocka.h>

void test_hashkeys_init_to_non_zero_value(void **state) {

    init_key_mgmt();
    uint64_t hash = hash_get_current_val();

    assert_true(hash != 0);
}

void test_hashkeys_all_pieces_all_squares_before_after_same(void **state) {

    init_key_mgmt();

    struct piece pce_array[NUM_PIECES];
    pce_get_all_pieces(pce_array);

    for (int p = 0; p < NUM_PIECES; p++) {
        struct piece pce = pce_array[p];

        for (enum square sq = a1; sq < h8; sq++) {

            // hash before
            uint64_t before_hash = hash_get_current_val();
            assert_true(before_hash != 0);

            // flip the hash for piece/square
            uint64_t after_init_flip_hash = hash_piece_update(pce, sq);
            assert_true(before_hash != after_init_flip_hash);

            // flip again
            uint64_t after_second_flip_hash = hash_piece_update(pce, sq);
            assert_true(after_init_flip_hash != after_second_flip_hash);

            // same as orig hash
            assert_true(before_hash == after_second_flip_hash);
        }
    }
}

void test_hashkeys_update_side(void **state) {

    init_key_mgmt();
    uint64_t init_hash = hash_get_current_val();

    uint64_t after_first_flip = hash_side_update();
    assert_true(init_hash != after_first_flip);

    uint64_t after_second_flip = hash_side_update();
    assert_true(after_first_flip != after_second_flip);

    assert_true(init_hash == after_second_flip);
}

void test_hashkeys_update_castle_permissions(void **state) {

    enum castle_permission cp_list[NUM_CASTLE_PERMS] = {CP_NONE, CP_WK, CP_WQ,
                                                        CP_BK, CP_BQ};
    init_key_mgmt();

    for (int i = 0; i < NUM_CASTLE_PERMS; i++) {
        enum castle_permission cp = cp_list[i];

        uint64_t init_hash = hash_get_current_val();

        // initial flip
        uint64_t after_init_flip = hash_castle_perm(cp);
        assert_true(after_init_flip != init_hash);

        // second flip
        uint64_t after_second_flip = hash_castle_perm(cp);
        assert_true(after_init_flip != after_second_flip);

        assert_true(init_hash == after_second_flip);
    }
}

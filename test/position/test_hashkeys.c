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
    struct hashkey hash = init_key_mgmt();

    assert_true(hash.hash != 0);
}

void test_hashkeys_all_pieces_all_squares_before_after_same(void **state) {

    enum piece pce_array[NUM_PIECES];
    pce_get_all_pieces(pce_array);

    for (int p = 0; p < NUM_PIECES; p++) {
        enum piece pce = pce_array[p];

        for (enum square sq = a1; sq < h8; sq++) {
            // hash before
            struct hashkey before_hash = init_key_mgmt();
            ;
            assert_true(before_hash.hash != 0);

            // flip the hash for piece/square
            struct hashkey after_init_flip_hash = hash_piece_update(pce, sq, before_hash);
            assert_true(before_hash.hash != after_init_flip_hash.hash);

            // flip again
            struct hashkey after_second_flip_hash = hash_piece_update(pce, sq, after_init_flip_hash);
            assert_true(after_init_flip_hash.hash != after_second_flip_hash.hash);

            // same as orig hash
            assert_true(before_hash.hash == after_second_flip_hash.hash);
        }
    }
}

void test_hashkeys_update_side(void **state) {

    struct hashkey init_hash = init_key_mgmt();

    struct hashkey after_first_flip = hash_side_update(init_hash);
    assert_true(init_hash.hash != after_first_flip.hash);

    struct hashkey after_second_flip = hash_side_update(after_first_flip);
    assert_true(after_first_flip.hash != after_second_flip.hash);

    assert_true(init_hash.hash == after_second_flip.hash);
}

void test_hashkeys_update_castle_permissions(void **state) {

    enum castle_permission cp_list[NUM_CASTLE_PERMS] = {CASTLE_PERM_NONE, CASTLE_PERM_WK, CASTLE_PERM_WQ, CASTLE_PERM_BK, CASTLE_PERM_BQ};

    for (int i = 0; i < NUM_CASTLE_PERMS; i++) {
        enum castle_permission cp = cp_list[i];

        struct hashkey init_hash = init_key_mgmt();

        // initial flip
        struct hashkey after_init_flip = hash_castle_perm(cp, init_hash);
        assert_true(after_init_flip.hash != init_hash.hash);

        // second flip
        struct hashkey after_second_flip = hash_castle_perm(cp, after_init_flip);
        assert_true(after_init_flip.hash != after_second_flip.hash);

        assert_true(init_hash.hash == after_second_flip.hash);
    }
}

void test_hashkeys_all_en_passant_squares_before_after_same(void **state) {

    for (enum square sq = a1; sq < h8; sq++) {

        // hash before
        struct hashkey before_hash = init_key_mgmt();
        assert_true(before_hash.hash != 0);

        // flip the hash for the square
        struct hashkey after_init_flip_hash = hash_en_passant(sq, before_hash);
        assert_true(before_hash.hash != after_init_flip_hash.hash);

        // flip again
        struct hashkey after_second_flip_hash = hash_en_passant(sq, after_init_flip_hash);
        assert_true(after_init_flip_hash.hash != after_second_flip_hash.hash);

        // same as orig hash
        assert_true(before_hash.hash == after_second_flip_hash.hash);
    }
}

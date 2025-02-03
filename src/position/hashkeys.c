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
/*! @addtogroup Hash
 *
 * @ingroup Hash
 * @{
 * @details Functions to hash board positions
 *
 */

#include "hashkeys.h"
#include "castle_perms.h"
#include "piece.h"
#include "rand.h"
#include "square.h"
#include "utils.h"
#include <assert.h>

static uint64_t piece_keys[NUM_PIECE_ROLES][NUM_COLOURS][NUM_SQUARES] = {{0}, {0}, {0}};
static uint64_t side_key = 0;
static uint64_t castle_keys[NUM_CASTLE_PERMS] = {0};
static uint64_t en_passant_sq_keys[NUM_SQUARES] = {0};

/**
 * @brief       Initialises the position hashkeys
 *
 */
uint64_t init_key_mgmt(void) {
    init_prng();

    uint64_t hashkey = 0;

    for (int num_roles = 0; num_roles < NUM_PIECE_ROLES; num_roles++) {
        for (int num_cols = 0; num_cols < NUM_COLOURS; num_cols++) {
            for (int num_sq = 0; num_sq < NUM_SQUARES; num_sq++) {

                const uint64_t rnd = genrand64_int64();
                piece_keys[num_roles][num_cols][num_sq] = rnd;

                hashkey ^= rnd;
            }
        }
    }

    side_key = genrand64_int64();
    hashkey ^= side_key;

    for (int num_sq = 0; num_sq < NUM_SQUARES; num_sq++) {
        en_passant_sq_keys[num_sq] = genrand64_int64();

        hashkey ^= en_passant_sq_keys[num_sq];
    }

    for (int i = 0; i < NUM_CASTLE_PERMS; i++) {
        castle_keys[i] = genrand64_int64();

        hashkey ^= castle_keys[i];
    }
    return hashkey;
}

uint64_t hash_piece_update(enum piece pce, enum square sq, uint64_t key_to_modify) {
    assert(validate_piece(pce));
    assert(validate_square(sq));

    const enum piece_role role = pce_get_role(pce);
    const enum colour colour = pce_get_colour(pce);

    return key_to_modify ^ piece_keys[role][colour][sq];
}

// bool hash_compare(const uint64_t hashkey1, const uint64_t hashkey2) {
//     return hashkey1 == hashkey2;
// }

uint64_t hash_piece_update_move(enum piece pce, enum square from_sq, enum square to_sq, uint64_t key_to_modify) {
    assert(validate_piece(pce));
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    const enum piece_role role = pce_get_role(pce);
    const enum colour colour = pce_get_colour(pce);

    uint64_t hashkey = key_to_modify ^ piece_keys[role][colour][from_sq];
    hashkey = key_to_modify ^ piece_keys[role][colour][to_sq];

    return hashkey;
}

/**
 * @brief               Flips the hash for the side
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
uint64_t hash_side_update(uint64_t key_to_modify) {
    return key_to_modify ^ side_key;
}

bool hash_compare(uint64_t hashkey1, uint64_t hashkey2) {
    return hashkey1 == hashkey2;
}

/**
 * @brief               Flips the hash for the en passant square
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
uint64_t hash_en_passant(enum square sq, uint64_t key_to_modify) {
    assert(validate_square(sq));
    return key_to_modify ^ en_passant_sq_keys[sq];
}

/**
 * @brief               Flips the hash based on the castle permissions
 * @param cp            The permissions 
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
uint64_t hash_castle_perm(enum castle_permission cp, uint64_t key_to_modify) {

    assert(validate_castle_permission(cp));

    const int cp_off = CP_ARRAY_OFFSET(cp);
    return key_to_modify ^ castle_keys[cp_off];
}

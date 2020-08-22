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
#include <assert.h>

static struct hashkey piece_keys[NUM_PIECES][NUM_SQUARES];
static struct hashkey side_key;
static struct hashkey castle_keys[NUM_CASTLE_PERMS];
static struct hashkey en_passant_sq_keys[NUM_SQUARES];

/**
 * @brief       Initialises the position hashkeys
 *
 */
struct hashkey init_key_mgmt(void) {
    init_prng();

    struct hashkey hashkey;

    hashkey.hash = 0;

    for (int num_pces = 0; num_pces < NUM_PIECES; num_pces++) {
        for (int num_sq = 0; num_sq < NUM_SQUARES; num_sq++) {
            const uint64_t rnd = genrand64_int64();
            piece_keys[num_pces][num_sq].hash = rnd;

            hashkey.hash ^= piece_keys[num_pces][num_sq].hash;
        }
    }

    side_key.hash = genrand64_int64();
    hashkey.hash ^= side_key.hash;

    for (int num_sq = 0; num_sq < NUM_SQUARES; num_sq++) {
        en_passant_sq_keys[num_sq].hash = genrand64_int64();

        hashkey.hash ^= en_passant_sq_keys[num_sq].hash;
    }

    for (int i = 0; i < NUM_CASTLE_PERMS; i++) {
        castle_keys[i].hash = genrand64_int64();

        hashkey.hash ^= castle_keys[i].hash;
    }
    return hashkey;
}

/**
 * @brief               Flips the hash based on the piece and square
 * @param pce           The piece 
 * @param sq            The square
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
struct hashkey hash_piece_update(const enum piece pce, const enum square sq, const struct hashkey key_to_modify) {

    assert(validate_piece(pce));
    assert(validate_square(sq));

    const uint8_t pce_off = pce_get_array_idx(pce);
    const uint64_t new_hash = key_to_modify.hash ^ piece_keys[pce_off][sq].hash;

    const struct hashkey retval = {.hash = new_hash};
    return retval;
}

/**
 * @brief               Flips the hash for the side
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
struct hashkey hash_side_update(const struct hashkey key_to_modify) {
    const uint64_t new_hash = key_to_modify.hash ^ side_key.hash;

    const struct hashkey retval = {.hash = new_hash};
    return retval;
}

/**
 * @brief               Flips the hash for the en passant square
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
struct hashkey hash_en_passant(const enum square sq, const struct hashkey key_to_modify) {
    assert(validate_square(sq));

    const uint64_t new_hash = key_to_modify.hash ^ en_passant_sq_keys[sq].hash;
    const struct hashkey retval = {.hash = new_hash};
    return retval;
}

/**
 * @brief               Flips the hash based on the castle permissions
 * @param pce           The piece 
 * @param sq            The square
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
struct hashkey hash_castle_perm(const enum castle_permission cp, const struct hashkey key_to_modify) {

    assert(validate_castle_permission(cp));

    const uint8_t cp_off = cast_perm_get_offset(cp);
    const uint64_t new_hash = key_to_modify.hash ^ castle_keys[cp_off].hash;
    const struct hashkey retval = {.hash = new_hash};
    return retval;
}

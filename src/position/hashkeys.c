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

static uint64_t piece_keys[NUM_PIECES][NUM_SQUARES] = {0};
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

    for (int num_pces = 0; num_pces < NUM_PIECES; num_pces++) {
        for (int num_sq = 0; num_sq < NUM_SQUARES; num_sq++) {
            const uint64_t rnd = genrand64_int64();
            piece_keys[num_pces][num_sq] = rnd;

            hashkey ^= piece_keys[num_pces][num_sq];
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

/**
 * @brief               Flips the hash based on the piece and square
 * @param pce           The piece 
 * @param sq            The square
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
uint64_t hash_piece_update(const enum piece pce, const enum square sq, const uint64_t key_to_modify) {
    assert(validate_piece(pce));
    assert(validate_square(sq));

    const uint8_t pce_off = PCE_GET_ARRAY_INDEX(pce);
    return key_to_modify ^ piece_keys[pce_off][sq];
}

bool hash_compare(const uint64_t hashkey1, const uint64_t hashkey2) {
    return hashkey1 == hashkey2;
}

/**
 * @brief               Flips the hash based on the piece and from- and to-square
 * @param pce           The piece 
 * @param from_sq       The from square
 * @param to_sq         The to square
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
uint64_t hash_piece_update_move(const enum piece pce, const enum square from_sq, const enum square to_sq,
                                const uint64_t key_to_modify) {
    assert(validate_piece(pce));
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    const uint8_t pce_off = PCE_GET_ARRAY_INDEX(pce);
    const uint64_t from_hash = key_to_modify ^ piece_keys[pce_off][from_sq];
    const uint64_t to_hash = from_hash ^ piece_keys[pce_off][to_sq];

    return to_hash;
}

/**
 * @brief               Flips the hash for the side
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
uint64_t hash_side_update(const uint64_t key_to_modify) {
    return key_to_modify ^ side_key;
}

/**
 * @brief               Flips the hash for the en passant square
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
uint64_t hash_en_passant(const enum square sq, const uint64_t key_to_modify) {
    assert(validate_square(sq));
    return key_to_modify ^ en_passant_sq_keys[sq];
}

/**
 * @brief               Flips the hash based on the castle permissions
 * @param pce           The piece 
 * @param sq            The square
 * @param key_to_modify The hashkey to modify
 * @return              The updated hash key
 */
uint64_t hash_castle_perm(const enum castle_permission cp, const uint64_t key_to_modify) {

    assert(validate_castle_permission(cp));

    const uint8_t cp_off = cast_perm_get_offset(cp);
    return key_to_modify ^ castle_keys[cp_off];
}

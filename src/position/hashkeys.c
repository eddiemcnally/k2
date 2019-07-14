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

#include "hashkeys.h"
#include "castle_perms.h"
#include "piece.h"
#include "rand.h"
#include "square.h"
#include <assert.h>

static uint64_t piece_keys[NUM_PIECES][NUM_SQUARES];
static uint64_t side_key;
static uint64_t castle_keys[NUM_CASTLE_PERMS];

static uint64_t hashkey;

/**
 * @brief       Initialises the position hashkeys
 *
 */
void init_key_mgmt(void) {
    init_prng();

    hashkey = 0;

    for (int num_pces = 0; num_pces < NUM_PIECES; num_pces++) {
        for (int num_sq = 0; num_sq < NUM_SQUARES; num_sq++) {
            piece_keys[num_pces][num_sq] = genrand64_int64();

            hashkey ^= piece_keys[num_pces][num_sq];
        }
    }

    side_key = genrand64_int64();
    hashkey ^= side_key;

    for (int i = 0; i < NUM_CASTLE_PERMS; i++) {
        castle_keys[i] = genrand64_int64();

        hashkey ^= castle_keys[i];
    }
}

/**
 * @brief       Flips the hash based on the piece and square
 * @param pce   The piece 
 * @param sq    The square
 * @return      The updated hash key
 */
uint64_t hash_piece_update(const struct piece pce, const enum square sq) {

    assert(validate_piece(pce));
    assert(validate_square(sq));

    enum piece_role pt = pce_get_piece_role(pce);
    uint8_t pce_off = pce_get_array_idx(pt);

    hashkey ^= piece_keys[pce_off][sq];
    return hashkey;
}

/**
 * @brief       Flips the hash for the side
 * @return      The updated hash key
 */
uint64_t hash_side_update(void) {
    hashkey ^= side_key;
    return hashkey;
}

/**
 * @brief       Flips the hash based on the castle permissions
 * @param pce   The piece 
 * @param sq    The square
 * @return      The updated hash key
 */
uint64_t hash_castle_perm(const enum castle_permission cp) {

    assert(validate_castle_permission(cp));

    uint8_t cp_off = cast_perm_get_offset(cp);
    hashkey ^= castle_keys[cp_off];
    return hashkey;
}

/**
 * @brief       Returns the current hash key
 * @return      The current hash key
 */
uint64_t hash_get_current_val(void) { return hashkey; }

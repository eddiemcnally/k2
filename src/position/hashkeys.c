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

static uint64_t piece_keys[NUM_PIECES][NUM_SQUARES];
static uint64_t side_key;
static uint64_t castle_keys[NUM_CASTLE_PERMS];

/**
 * @brief       Initialises the position hashkeys
 *
 */
void init_key_mgmt(void) {
    // init the prng
    init_prng();

    for (int num_pces = 0; num_pces < NUM_PIECES; num_pces++) {
        for (int num_sq = 0; num_sq < NUM_SQUARES; num_sq++) {
            piece_keys[num_pces][num_sq] = genrand64_int64();
        }
    }

    side_key = genrand64_int64();

    for (int i = 0; i < NUM_CASTLE_PERMS; i++) {
        castle_keys[i] = genrand64_int64();
    }
}

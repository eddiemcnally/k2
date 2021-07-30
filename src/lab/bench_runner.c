/*  MIT License
 *
 *  Copyright (c) 2021 Eddie McNally
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

#include "board.h"
#include "move.h"
#include "move_gen.h"
#include "move_list.h"
#include "perft.h"
#include "piece.h"
#include "position.h"
#include "search.h"
#include "square.h"
#include "utils.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MILLION 1000000
#define NUM_LOOPS (100 * MILLION)
#define NUM_TESTS 20

static uint64_t first(void);
static uint64_t second(void);

static void set_square_1(uint64_t *const bb, const enum square sq);
static uint64_t set_square_2(uint64_t const bb, const enum square sq);

int main(void) {

    uint64_t accum_1 = 0;
    uint64_t accum_2 = 0;

    for (int i = 0; i < NUM_TESTS; i++) {

        struct timespec start_1, end_1;
        struct timespec start_2, end_2;
        double elapsed_sec_1, elapsed_sec_2;

        clock_gettime(CLOCK_MONOTONIC, &start_1);
        for (int j = 0; j < NUM_LOOPS; j++) {
            accum_1 += first();
        }
        clock_gettime(CLOCK_MONOTONIC, &end_1);
        elapsed_sec_1 = (end_1.tv_sec + end_1.tv_nsec - start_1.tv_sec + start_1.tv_nsec) / 1000000000.0;
        printf("Test 1 : %lf\n", elapsed_sec_1);

        clock_gettime(CLOCK_MONOTONIC, &start_2);
        for (int j = 0; j < NUM_LOOPS; j++) {
            accum_2 += second();
        }
        clock_gettime(CLOCK_MONOTONIC, &end_2);
        elapsed_sec_2 = (end_2.tv_sec + end_2.tv_nsec - start_2.tv_sec + start_2.tv_nsec) / 1000000000.0;
        printf("Test 2 : %lf\n", elapsed_sec_2);
    }
    REQUIRE(accum_2 == accum_1, "should be the same");
}

static uint64_t first(void) {

    uint64_t bb = 0;
    for (enum square sq = a1; sq <= h8; sq++) {
        set_square_1(&bb, sq);
    }
    REQUIRE(bb == 0xFFFFFFFFFFFFFFFF, "problem 1");
    return bb;
}

static uint64_t second(void) {
    uint64_t bb = 0;
    for (enum square sq = a1; sq <= h8; sq++) {
        bb = set_square_2(bb, sq);
    }
    REQUIRE(bb == 0xFFFFFFFFFFFFFFFF, "problem 1");
    return bb;
}

__attribute__((always_inline)) static void set_square_1(uint64_t *const bb, const enum square sq) {
    *bb = *bb | ((uint64_t)0x01 << sq);
}

__attribute__((always_inline)) static uint64_t set_square_2(uint64_t const bb, const enum square sq) {
    return bb | ((uint64_t)0x01 << sq);
}

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

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"

#include "test_perft.h"
#include "perft.h"
#include "perft_file_reader.h"
#include "position.h"
#include <cmocka.h>

void test_perft_1(void **state) {
    char *PERFT =
        "r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1 ;D1 25 ;D2 560 ;D3 13607 ;D4 320792 ;D5 7848606 ;D6 190755813\n";

    const uint8_t DEPTH = 5;
    struct epd_row perft_details = perft_parse_row(PERFT);
    struct position *pos = pos_create();

    pos_initialise(perft_details.fen, pos);

    const uint64_t expected_nodes = perft_details.move_cnt[DEPTH - 1];
    const uint64_t actual_nodes = do_perft(DEPTH, pos);

    assert_true(expected_nodes == actual_nodes);
    pos_destroy(pos);
}

void test_perft_2(void **state) {
    char *PERFT = "R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1 ;D1 36 ;D2 1027 ;D3 29215 ;D4 771461 ;D5 20506480 ;D6 525169084";

    const uint8_t DEPTH = 5;
    struct epd_row perft_details = perft_parse_row(PERFT);
    struct position *pos = pos_create();

    pos_initialise(perft_details.fen, pos);

    const uint64_t expected_nodes = perft_details.move_cnt[DEPTH - 1];
    const uint64_t actual_nodes = do_perft(DEPTH, pos);

    assert_true(expected_nodes == actual_nodes);
    pos_destroy(pos);
}

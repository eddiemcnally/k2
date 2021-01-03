/*  MIT License
 *
 *  Copyright (c) 2020 Eddie McNally
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

#include "test_basic_evaluator.h"
#include "basic_evaluator.h"
#include <cmocka.h>

void test_basic_evaluator_sample_white_position(void **state) {
    const char *FEN = "k7/8/1P3B2/P6P/3Q4/1N6/3K4/7R w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    int32_t score = evaluate_position_basic(pos);

    assert_true(score == 2350);
}

void test_basic_evaluator_sample_black_position(void **state) {
    const char *FEN = "1k6/1pp3q1/5b2/1n6/7p/8/3K4/8 b - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    int32_t score = evaluate_position_basic(pos);

    assert_true(score == 1980);
}

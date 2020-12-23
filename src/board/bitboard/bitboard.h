/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
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

#pragma once

#include "square.h"
#include <stdbool.h>
#include <stdint.h>

uint64_t bb_get_empty(void);
uint64_t bb_get_sq_mask(const enum square sq);
uint64_t bb_set_square(const uint64_t bb, const enum square sq);
uint64_t bb_get_square_as_bb(const enum square sq);
uint64_t bb_clear_square(const uint64_t bb, const enum square sq);
bool bb_is_set(const uint64_t bb, const enum square sq);
bool bb_is_clear(const uint64_t bb, const enum square sq);
enum square bb_pop_1st_bit(const uint64_t bb);
uint64_t bb_reverse(uint64_t bb);
void bb_print_as_board(const uint64_t bb);

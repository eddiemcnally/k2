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

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void test_position_get_set_castle_permissions(void **state);
void test_position_brd_is_sq_occupied(void **state);
void test_position_compare(void **state);
void test_position_make_move_white_double_first_move(void **state);
void test_position_make_move_black_double_first_move(void **state);
void test_position_make_move_castle_white_kingside_move_valid_position_updated(
    void **state);
void test_position_make_move_castle_white_queenside_move_valid_position_updated(
    void **state);
void test_position_make_move_castle_black_kingside_move_valid_position_updated(
    void **state);
void test_position_make_move_castle_black_queenside_move_valid_position_updated(
    void **state);
void test_position_make_move_castle_white_kingside_move_invalid(void **state);
void test_position_make_move_castle_white_queenside_move_invalid(void **state);
void test_position_make_move_castle_black_kingside_move_invalid(void **state);
void test_position_make_move_castle_black_queenside_move_invalid(void **state);
void test_position_make_move_white_knight(void **state);
void test_position_make_move_black_knight(void **state);
void test_position_make_move_white_bishop(void **state);
void test_position_make_move_black_bishop(void **state);
void test_position_make_move_black_queen(void **state);
void test_position_make_move_white_queen(void **state);
void test_position_make_move_white_discovered_attack_on_king_invalid_move(
    void **state);
void test_position_make_move_black_discovered_attack_on_king_invalid_move(
    void **state);

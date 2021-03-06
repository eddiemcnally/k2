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
#include "bitboard.h"
#include "piece.h"
#include "square.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct board;

struct board *brd_allocate(void);
void brd_deallocate(struct board *brd);
bool validate_board(const struct board *brd);

bool brd_is_sq_occupied(const struct board *brd, const enum square sq);
enum piece brd_get_piece_on_square(const struct board *brd, const enum square sq);
void brd_add_piece(struct board *brd, const enum piece pce, const enum square sq);
uint64_t brd_get_board_bb(const struct board *brd);
uint64_t brd_get_piece_bb(const struct board *brd, const enum piece pce);

enum square brd_get_white_king_square(const struct board *brd);
enum square brd_get_black_king_square(const struct board *brd);

uint64_t brd_get_white_rook_queen_bb(const struct board *brd);
uint64_t brd_get_black_rook_queen_bb(const struct board *brd);
uint64_t brd_get_white_bishop_queen_bb(const struct board *brd);
uint64_t brd_get_black_bishop_queen_bb(const struct board *brd);

void brd_remove_piece(struct board *brd, const enum piece pce, const enum square sq);
void brd_remove_from_square(struct board *brd, const enum square sq);
void brd_move_piece(struct board *brd, const enum piece pce, const enum square from_sq, const enum square to_sq);
uint64_t brd_get_colour_bb(const struct board *brd, const enum colour colour);
bool brd_compare(const struct board *first, const struct board *second);
int32_t brd_get_material(const struct board *brd, const enum colour side);

void brd_print(const struct board *brd);

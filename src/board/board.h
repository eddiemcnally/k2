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

struct material {
    Score white;
    Score black;
};

struct board;

void brd_deallocate(struct board *const brd);
bool validate_board(const struct board *const brd);
bool brd_is_sq_occupied(const struct board *const brd, enum square sq);
uint64_t brd_get_board_bb(const struct board *const brd);
uint64_t brd_get_piece_bb(const struct board *const brd, enum piece pce);

bool brd_try_get_colour_on_sq(const struct board *const brd, enum square sq, enum colour *colour);

void brd_add_piece(struct board *const brd, enum piece pce, enum square sq);
void brd_remove_piece(struct board *const brd, enum piece pce, enum square sq);
void brd_remove_from_square(struct board *const brd, enum square sq);
void brd_move_piece(struct board *const brd, enum piece pce, enum square from_sq, enum square to_sq);
uint64_t brd_get_colour_bb(const struct board *const brd, enum colour colour);

uint64_t brd_get_bb_for_role_colour(const struct board *const brd, enum piece_role role, enum colour colour);

bool brd_compare(const struct board *const first, const struct board *const second);
void brd_print(const struct board *const brd);

enum square brd_get_king_square(const struct board *const brd, enum colour colour);

bool brd_try_get_piece_on_square(const struct board *const brd, enum square sq, enum piece *piece);
struct material brd_get_material(const struct board *const brd);
struct board *brd_allocate(void);

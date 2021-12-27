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
#include "square.h"
#include <stdint.h>

// clang-format off

struct diagonals {
    uint64_t positive;
    uint64_t negative;
};

#define RANK_MASK   ((uint64_t)0x00000000000000ff)
#define FILE_MASK   ((uint64_t)0x0101010101010101)

#define FILE_A_BB   ((uint64_t)(FILE_MASK << (int)FILE_A))
#define FILE_H_BB   ((uint64_t)(FILE_MASK << (int)FILE_H))

#define NORTH_EAST(bb)  ((uint64_t)((bb & ~FILE_H_BB) << 9))
#define SOUTH_EAST(bb)  ((uint64_t)((bb & ~FILE_H_BB) >> 7))
#define NORTH_WEST(bb)  ((uint64_t)((bb & ~FILE_A_BB) << 7))
#define SOUTH_WEST(bb)  ((uint64_t)((bb & ~FILE_A_BB) >> 9))

#define NORTH(bb)       ((uint64_t)(bb << 8))
#define SOUTH(bb)       ((uint64_t)(bb >> 8))

// clang-format on

void occ_mask_init(void);
uint64_t occ_mask_get_inbetween(const enum square sq1, const enum square sq2);
uint64_t occ_mask_get_black_pawn_capture_non_first_double_move(const enum square sq);
uint64_t occ_mask_get_white_pawn_capture_non_first_double_move(const enum square sq);
uint64_t occ_mask_get_knight(const enum square sq);
uint64_t occ_mask_get_bishop(const enum square sq);
uint64_t occ_mask_get_king(const enum square sq);
uint64_t occ_mask_get_queen(const enum square sq);
uint64_t occ_mask_get_rook(const enum square sq);
uint64_t occ_mask_get_vertical(const enum square sq);
uint64_t occ_mask_get_horizontal(const enum square sq);
uint64_t occ_mask_get_bb_white_pawns_attacking_sq(const enum square sq);
uint64_t occ_mask_get_bb_black_pawns_attacking_sq(const enum square sq);
uint64_t occ_mask_get_double_pawn_mask_white(const enum square from_sq);
uint64_t occ_mask_get_double_pawn_mask_black(const enum square from_sq);

struct diagonals occ_mask_get_diagonals(const enum square sq);

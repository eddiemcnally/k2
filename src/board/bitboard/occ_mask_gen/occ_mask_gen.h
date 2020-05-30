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

#include "bitboard.h"
#include "board.h"
#include "piece.h"
#include "square.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void occ_mask_gen_ranks_files(void);
void occ_mask_gen_king(uint64_t *occ_mask_array);
void occ_mask_gen_knight(uint64_t *occ_mask_array);
void occ_mask_gen_rook(uint64_t *occ_mask_array);
void occ_mask_gen_bishop(uint64_t *occ_mask_array);
void occ_mask_gen_queen(uint64_t *occ_mask_array);
void occ_mask_gen_white_pawn_capture_non_first_double_move(uint64_t *occ_mask_array);
void occ_mask_gen_black_pawn_capture_non_first_double_move(uint64_t *occ_mask_array);
void occ_mask_gen_print_as_board(const uint64_t mask);
void occ_mask_gen_print_all_as_hex(const uint64_t masks[], const uint8_t size);
void occ_mask_gen_print_as_hex(const uint64_t mask);
void occ_mask_gen_diagonal_occupancy_masks(void);

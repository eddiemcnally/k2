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

#include "piece.h"
#include "position.h"
#include "square.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct parsed_fen;

struct parsed_fen *fen_parse(const char *fen);

enum piece fen_get_piece_on_sq(const struct parsed_fen *const pf, enum square sq);
bool fen_has_wk_castle_perms(const struct parsed_fen *const pf);
bool fen_has_wq_castle_perms(const struct parsed_fen *const pf);
bool fen_has_bk_castle_perms(const struct parsed_fen *const pf);
bool fen_has_bq_castle_perms(const struct parsed_fen *const pf);
enum square fen_get_en_pass_sq(const struct parsed_fen *const pf);
enum colour fen_get_side_to_move(const struct parsed_fen *const pf);
uint16_t fen_get_half_move_cnt(const struct parsed_fen *const pf);
uint16_t fen_get_full_move_cnt(const struct parsed_fen *const pf);

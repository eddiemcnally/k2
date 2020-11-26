/*  MIT License
 *
 *  Copyright (c) 2019 Eddie McNally
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

#include "castle_perms.h"
#include "hashkeys.h"
#include "move.h"
#include "position.h"
#include <stdint.h>

struct position_hist;

struct position_hist *position_hist_init(void);
void position_hist_release_memory(struct position_hist *mh);

void position_hist_push(struct position_hist *move_history, const struct move mv, const uint8_t fifty_move_counter,
                        const enum square en_passant_sq, const struct hashkey hashkey,
                        const struct cast_perm_container castle_perm_container, const struct board *brd);

void position_hist_pop(struct position_hist *pos_history, struct move *mv, uint8_t *fifty_move_counter,
                       enum square *en_passant_sq, struct hashkey *hashkey,
                       struct cast_perm_container *castle_perm_container, struct board *brd);

bool position_hist_compare(const struct position_hist *hist1, const struct position_hist *hist2);

uint16_t position_hist_get_num(const struct position_hist *mh);

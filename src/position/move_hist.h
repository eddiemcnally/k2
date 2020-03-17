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
#include "move.h"
#include "position.h"
#include <stdint.h>

struct move_hist;

struct move_hist *move_hist_init(void);
void move_hist_release_memory(struct move_hist *mh);

void move_hist_push(struct move_hist *move_history, const struct move mv,
                    const uint8_t fifty_move_counter,
                    const struct en_pass_active en_passant,
                    const uint64_t hashkey,
                    const struct cast_perm_container castle_perm_container);

void move_hist_pop(struct move_hist *move_history, struct move *mv,
                   uint8_t *fifty_move_counter,
                   struct en_pass_active *en_passant, uint64_t *hashkey,
                   struct cast_perm_container *castle_perm_container);

bool move_hist_compare(const struct move_hist *hist1,
                       const struct move_hist *hist2);

uint16_t move_hist_get_num(const struct move_hist *mh);
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
#include "move.h"
#include <stdint.h>

// struct representing a list of moves
struct move_list;

struct move_list *mvl_allocate(void);
void mvl_deallocate(struct move_list *mvl);
uint16_t mvl_get_move_count(const struct move_list *mvl);
struct move mvl_get_move_at_offset(const struct move_list *mvl,
                                   uint16_t offset);
void mvl_add(struct move_list *mvl, struct move mv);
void mvl_reset(struct move_list *mvl);
bool mvl_contains_move(const struct move_list *mvl, const struct move mv);

uint16_t mvl_get_mvl_max_size(void);
bool validate_move_list(const struct move_list *mvl);
void mvl_print(const struct move_list *mvl);
bool mvl_compare(const struct move_list *first, const struct move_list *second);

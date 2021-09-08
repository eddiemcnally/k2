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

#define MOVE_LIST_MAX_LEN 2048

// struct representing a list of moves
struct move_list {
    uint16_t struct_init_key;
    uint16_t move_count;
    struct move move_list[MOVE_LIST_MAX_LEN];
};

struct move_list mvl_initialise(void);
uint16_t mvl_get_move_count(const struct move_list *const mvl);
struct move mvl_get_move_at_offset(const struct move_list *const mvl, uint16_t offset);
void mvl_add(struct move_list *const mvl, struct move mv);
void mvl_reset(struct move_list *const mvl);
bool mvl_contains_move(const struct move_list *const mvl, const struct move mv);

uint16_t mvl_get_mvl_max_size(void);
bool validate_move_list(const struct move_list *const mvl);
void mvl_print(const struct move_list *const mvl);
bool mvl_compare(const struct move_list *const first, const struct move_list *const second);
void mvl_move_highest_score_to_start_of_slice(struct move_list *const mvl, uint32_t slice_start_index);

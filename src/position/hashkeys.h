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
#include "piece.h"
#include "square.h"
#include <stdint.h>

struct hashkey {
    uint64_t hash;
};

struct hashkey init_key_mgmt(void);
bool hash_compare(const struct hashkey hashkey1, const struct hashkey hashkey2);
struct hashkey hash_piece_update(const enum piece pce, const enum square sq, const struct hashkey key_to_modify);
struct hashkey hash_piece_update_move(const enum piece pce, const enum square from_sq, const enum square to_sq,
                                      const struct hashkey key_to_modify);
struct hashkey hash_side_update(const struct hashkey key_to_modify);
struct hashkey hash_castle_perm(const enum castle_permission cp, const struct hashkey key_to_modify);
struct hashkey hash_en_passant(const enum square sq, const struct hashkey key_to_modify);

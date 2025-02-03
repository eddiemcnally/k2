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

uint64_t init_key_mgmt(void);
bool hash_compare(uint64_t hashkey1, uint64_t hashkey2);
uint64_t hash_piece_update(enum piece pce, enum square sq, uint64_t key_to_modify);
uint64_t hash_piece_update_move(enum piece pce, enum square from_sq, enum square to_sq, uint64_t key_to_modify);
uint64_t hash_side_update(uint64_t key_to_modify);
uint64_t hash_castle_perm(enum castle_permission cp, uint64_t key_to_modify);
uint64_t hash_en_passant(enum square sq, uint64_t key_to_modify);

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

#include "castle_perms.h"
#include "move.h"
#include "piece.h"
#include "square.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct position;

#define MAX_GAME_MOVES (1024)

// clang-format off

enum move_legality { 
    LEGAL_MOVE, 
    ILLEGAL_MOVE 
};

#define INITIAL_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n"

// clang-format on

struct cast_perm_container pos_get_cast_perm(const struct position *const pos);
void pos_set_cast_perm(struct position *const pos, const struct cast_perm_container perms);

struct position *pos_create(void);
void pos_destroy(struct position *pos);
void pos_initialise(const char *const fen, struct position *const pos);

struct board *pos_get_board(const struct position *const pos);
enum square pos_get_en_pass_sq(const struct position *const pos);
bool pos_is_en_passant_active(const struct position *const pos);
enum colour pos_get_side_to_move(const struct position *const pos);

enum move_legality pos_make_move(struct position *const pos, const struct move mv);
struct move pos_take_move(struct position *const pos);

bool validate_position(const struct position *const pos);
bool pos_compare(const struct position *const first, const struct position *const second);

uint16_t pos_get_ply(const struct position *const pos);

uint64_t pos_get_hash(const struct position *const pos);

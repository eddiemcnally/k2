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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "piece.h"
#include "square.h"

// struct representing a single move
struct move {
    uint16_t val;
};

struct move move_encode_quiet(const enum square from_sq, const enum square to_sq);
struct move move_encode_promoted(const enum square from_sq, const enum square to_sq,
                                 const enum piece_role promoted_piece, const bool is_capture);
struct move move_encode_capture(const enum square from_sq, const enum square to_sq);
struct move move_encode_enpassant(const enum square from_sq, const enum square to_sq);
struct move move_encode_pawn_double_first(const enum square from_sq, const enum square to_sq);
struct move move_encode_castle_kingside(const enum colour side);
struct move move_encode_castle_queenside(const enum colour side);

bool move_compare(const struct move mv1, const struct move mv2);

enum square move_decode_from_sq(const struct move mv);
enum square move_decode_to_sq(const struct move mv);
bool try_move_decode_promotion_piece(const struct move mv, const enum colour side, enum piece *pce);
bool move_is_quiet(const struct move mv);
bool move_is_capture(const struct move mv);
bool move_is_promotion(const struct move mv);
bool move_is_en_passant(const struct move mv);
bool move_is_castle(const struct move mv);
bool move_is_double_pawn(const struct move mv);
bool move_is_king_castle(const struct move mv);
bool move_is_queen_castle(const struct move mv);
char *move_print(struct move mv);

bool validate_move(const struct move mv);

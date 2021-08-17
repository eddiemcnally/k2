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
    uint64_t val;
};

/**
 * bitmap for move
 * See https://www.chessprogramming.org/Encoding_Moves
 *
 * 0000     Quiet move
 * 0001     Double Pawn push
 * 0010     King Castle
 * 0011     Queen Castle
 * 0100     Capture
 * 0101     En Passant Capture
 * 1000     Promotion Knight
 * 1001     Promotion Bishop
 * 1010     Promotion Rook
 * 1011     Promotion Queen
 * 1100     Promotion Knight Capture
 * 1101     Promotion Bishop Capture
 * 1110     Promotion Rook Capture
 * 1111     Promotion Queen Capture
 */

enum move_type {
    MV_TYPE_QUIET = 0x0,
    MV_TYPE_DOUBLE_PAWN = 0x1,
    MV_TYPE_KING_CASTLE = 0x2,
    MV_TYPE_QUEEN_CASTLE = 0x3,
    MV_TYPE_CAPTURE = 0x4,
    MV_TYPE_EN_PASS = 0x5,
    MV_TYPE_PROMOTE_KNIGHT = 0x8,
    MV_TYPE_PROMOTE_BISHOP = 0x9,
    MV_TYPE_PROMOTE_ROOK = 0xA,
    MV_TYPE_PROMOTE_QUEEN = 0xB,
    MV_TYPE_PROMOTE_KNIGHT_CAPTURE = 0xC,
    MV_TYPE_PROMOTE_BISHOP_CAPTURE = 0xD,
    MV_TYPE_PROMOTE_ROOK_CAPTURE = 0xE,
    MV_TYPE_PROMOTE_QUEEN_CAPTURE = 0xF,
};

struct move move_encode_promote_knight(const enum square from_sq, const enum square to_sq);
struct move move_encode_promote_knight_with_capture(const enum square from_sq, const enum square to_sq);
struct move move_encode_promote_bishop(const enum square from_sq, const enum square to_sq);
struct move move_encode_promote_bishop_with_capture(const enum square from_sq, const enum square to_sq);
struct move move_encode_promote_rook(const enum square from_sq, const enum square to_sq);
struct move move_encode_promote_rook_with_capture(const enum square from_sq, const enum square to_sq);
struct move move_encode_promote_queen(const enum square from_sq, const enum square to_sq);
struct move move_encode_promote_queen_with_capture(const enum square from_sq, const enum square to_sq);

struct move move_encode_quiet(const enum square from_sq, const enum square to_sq);
struct move move_encode_capture(const enum square from_sq, const enum square to_sq);
struct move move_encode_enpassant(const enum square from_sq, const enum square to_sq);
struct move move_encode_pawn_double_first(const enum square from_sq, const enum square to_sq);
struct move move_encode_castle_kingside_white(void);
struct move move_encode_castle_kingside_black(void);
struct move move_encode_castle_queenside_white(void);
struct move move_encode_castle_queenside_black(void);

enum move_type move_get_type(const struct move mv);

bool move_compare(const struct move mv1, const struct move mv2);

struct move move_get_no_move(void);

enum move_type move_get_move_type(const struct move mv);
enum square move_decode_from_sq(const struct move mv);
enum square move_decode_to_sq(const struct move mv);
bool move_is_quiet(const struct move mv);
bool move_is_capture(const struct move mv);
bool move_is_promotion(const struct move mv);
bool move_is_en_passant(const struct move mv);
bool move_is_castle(const struct move mv);
bool move_is_double_pawn(const struct move mv);
bool move_is_king_castle(const struct move mv);
bool move_is_queen_castle(const struct move mv);
char *move_print(struct move mv);

void move_set_score(struct move *const mv, const int32_t score);
int32_t move_get_score(const struct move mv);

bool validate_move(const struct move mv);

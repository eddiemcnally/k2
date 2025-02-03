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
#include "score.h"
#include "square.h"

struct move {
    uint16_t bitmap;
};

/**
 * bitmap for move type
 * See https://www.chessprogramming.org/Encoding_Moves
 *
 * 0000     Quiet move
 * 0001     Double Pawn push
 * 0010     King Castle
 * 0011     Queen Castle
 * 0100     Capture
 * 0101     En Passant Capture
 * 0110     UNUSED
 * 0111     UNUSED
 * 1000     Promotion Knight
 * 1001     Promotion Bishop
 * 1010     Promotion Rook
 * 1011     Promotion Queen
 * 1100     Promotion Knight Capture
 * 1101     Promotion Bishop Capture
 * 1110     Promotion Rook Capture
 * 1111     Promotion Queen Capture
 */
// clang-format off
enum move_type {
    MV_TYPE_QUIET                   = 0x0000,
    MV_TYPE_DOUBLE_PAWN             = 0x1000,
    MV_TYPE_KING_CASTLE             = 0x2000,
    MV_TYPE_QUEEN_CASTLE            = 0x3000,
    MV_TYPE_CAPTURE                 = 0x4000,
    MV_TYPE_EN_PASS                 = 0x5000,
    MV_TYPE_PROMOTE_KNIGHT          = 0x8000,
    MV_TYPE_PROMOTE_BISHOP          = 0x9000,
    MV_TYPE_PROMOTE_ROOK            = 0xA000,
    MV_TYPE_PROMOTE_QUEEN           = 0xB000,
    MV_TYPE_PROMOTE_KNIGHT_CAPTURE  = 0xC000,
    MV_TYPE_PROMOTE_BISHOP_CAPTURE  = 0xD000,
    MV_TYPE_PROMOTE_ROOK_CAPTURE    = 0xE000,
    MV_TYPE_PROMOTE_QUEEN_CAPTURE   = 0xF000,
};
// clang-format on

struct move move_encode_promote_knight(enum square from_sq, enum square to_sq);
struct move move_encode_promote_knight_with_capture(enum square from_sq, enum square to_sq);
struct move move_encode_promote_bishop(enum square from_sq, enum square to_sq);
struct move move_encode_promote_bishop_with_capture(enum square from_sq, enum square to_sq);
struct move move_encode_promote_rook(enum square from_sq, enum square to_sq);
struct move move_encode_promote_rook_with_capture(enum square from_sq, enum square to_sq);
struct move move_encode_promote_queen(enum square from_sq, enum square to_sq);
struct move move_encode_promote_queen_with_capture(enum square from_sq, enum square to_sq);

struct move move_encode_quiet(enum square from_sq, enum square to_sq);
struct move move_encode_capture(enum square from_sq, enum square to_sq);
struct move move_encode_enpassant(enum square from_sq, enum square to_sq);
struct move move_encode_pawn_double_first(enum square from_sq, enum square to_sq);
struct move move_encode_castle_kingside_white(void);
struct move move_encode_castle_kingside_black(void);
struct move move_encode_castle_queenside_white(void);
struct move move_encode_castle_queenside_black(void);

enum move_type move_get_type(struct move mv);

bool move_compare(struct move mv1, struct move mv2);

struct move move_get_no_move(void);

enum move_type move_get_move_type(struct move mv);
enum square move_decode_from_sq(struct move mv);
enum square move_decode_to_sq(struct move mv);
bool move_is_quiet(struct move mv);
bool move_is_capture(struct move mv);
bool move_is_promotion(struct move mv);
bool move_is_en_passant(struct move mv);
bool move_is_castle(struct move mv);
bool move_is_double_pawn(struct move mv);
bool move_is_king_castle(struct move mv);
bool move_is_queen_castle(struct move mv);
char *move_print(struct move mv);

bool validate_move(struct move mv);

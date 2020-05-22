/*  MIT License
 *
 *  Copyright (c) 2020 Eddie McNally
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

#include "evaluate.h"
#include "bitboard.h"
#include "board.h"
#include "piece.h"
#include "position.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static int32_t eval_white_pieces_on_square(void);
static int32_t eval_black_pieces_on_square(void);

// Values for piece square arrays are taken from
// https://www.chessprogramming.org/Simplified_Evaluation_Function
//
static const int8_t PAWN_SQ_VALUE[NUM_SQUARES] = {
    0,  0,  0,  0,   0,   0,  0,  0,  50, 50, 50,  50, 50, 50,  50, 50,
    10, 10, 20, 30,  30,  20, 10, 10, 5,  5,  10,  25, 25, 10,  5,  5,
    0,  0,  0,  20,  20,  0,  0,  0,  5,  -5, -10, 0,  0,  -10, -5, 5,
    5,  10, 10, -20, -20, 10, 10, 5,  0,  0,  0,   0,  0,  0,   0,  0};

static const int8_t KNIGHT_SQ_VALUE[NUM_SQUARES] = {
    -50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0,   0,   0,
    0,   -20, -40, -30, 0,   10,  15,  15,  10,  0,   -30, -30, 5,
    15,  20,  20,  15,  5,   -30, -30, 0,   15,  20,  20,  15,  0,
    -30, -30, 5,   10,  15,  15,  10,  5,   -30, -40, -20, 0,   5,
    5,   0,   -20, -40, -50, -40, -30, -30, -30, -30, -40, -50};

static const int8_t BISHOP_SQ_VALUE[NUM_SQUARES] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 0,   0,   0,   0,
    0,   0,   -10, -10, 0,   5,   10,  10,  5,   0,   -10, -10, 5,
    5,   10,  10,  5,   5,   -10, -10, 0,   10,  10,  10,  10,  0,
    -10, -10, 10,  10,  10,  10,  10,  10,  -10, -10, 5,   0,   0,
    0,   0,   5,   -10, -20, -10, -10, -10, -10, -10, -10, -20,
};

static const int8_t ROOK_SQ_VALUE[NUM_SQUARES] = {
    0,  0, 0, 0, 0, 0, 0, 0,  5,  10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5, -5, 0,  0,  0,  0,  0,  0,  -5,
    -5, 0, 0, 0, 0, 0, 0, -5, -5, 0,  0,  0,  0,  0,  0,  -5,
    -5, 0, 0, 0, 0, 0, 0, -5, 0,  0,  0,  5,  5,  0,  0,  0,
};

static const int8_t QUEEN_SQ_VALUE[NUM_SQUARES] = {
    -20, -10, -10, -5, -5, -10, -10, -20, -10, 0,   0,   0,  0,  0,   0,   -10,
    -10, 0,   5,   5,  5,  5,   0,   -10, -5,  0,   5,   5,  5,  5,   0,   -5,
    0,   0,   5,   5,  5,  5,   0,   -5,  -10, 5,   5,   5,  5,  5,   0,   -10,
    -10, 0,   5,   0,  0,  0,   0,   -10, -20, -10, -10, -5, -5, -10, -10, -20,
};

static const int8_t KING_SQ_VALUE[NUM_SQUARES] = {
    -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50,
    -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40,
    -40, -50, -50, -40, -40, -30, -20, -30, -30, -40, -40, -30, -30,
    -20, -10, -20, -20, -20, -20, -20, -20, -10, 20,  20,  0,   0,
    0,   0,   20,  20,  20,  30,  10,  0,   0,   10,  30,  20,
};

// ToDo - add game state and swap to this array
//
// static const int8_t KING_SQ_ENDGAME_VALUE [] = {
//     -50, -40, -30, -20, -20, -30, -40, -50, -30, -20, -10, 0, 0, -10, -20, -30, -30, -10, 20, 30,
//     30, 20, -10, -30, -30, -10, 30, 40, 40, 30, -10, -30, -30, -10, 30, 40, 40, 30, -10, -30, -30,
//     -10, 20, 30, 30, 20, -10, -30, -30, -30, 0, 0, 0, 0, -30, -30, -50, -30, -30, -30, -30, -30,
//     -30, -50,
// };

static const uint8_t MIRROR_VALUE[NUM_SQUARES] = {
    56, 57, 58, 59, 60, 61, 62, 63, 48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47, 32, 33, 34, 35, 36, 37, 38, 39,
    24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23,
    8,  9,  10, 11, 12, 13, 14, 15, 0,  1,  2,  3,  4,  5,  6,  7,
};

struct value_info {
    uint64_t bb;
    const int8_t (*val_array)[NUM_SQUARES];
};

#define EV_PAWN_OFF 0
#define EV_BISHOP_OFF 1
#define EV_KNIGHT_OFF 2
#define EV_ROOK_OFF 3
#define EV_QUEEN_OFF 4
#define EV_KING_OFF 5

static struct value_info val_info[NUM_PIECE_ROLES] = {
    [EV_PAWN_OFF] = {.bb = 0, .val_array = &PAWN_SQ_VALUE},
    [EV_BISHOP_OFF] = {.bb = 0, .val_array = &BISHOP_SQ_VALUE},
    [EV_KNIGHT_OFF] = {.bb = 0, .val_array = &KNIGHT_SQ_VALUE},
    [EV_ROOK_OFF] = {.bb = 0, .val_array = &ROOK_SQ_VALUE},
    [EV_QUEEN_OFF] = {.bb = 0, .val_array = &QUEEN_SQ_VALUE},
    [EV_KING_OFF] = {.bb = 0, .val_array = &KING_SQ_VALUE},
};

int32_t evaluate_position(const struct position *pos) {

    const struct board *brd = pos_get_board(pos);

    const uint32_t white_material = brd_get_material(brd, WHITE);
    const uint32_t black_material = brd_get_material(brd, BLACK);

    int32_t score = (int32_t)(white_material - black_material);

    // populate bitboards for white
    val_info[EV_PAWN_OFF].bb = brd_get_piece_bb(brd, WHITE_PAWN);
    val_info[EV_BISHOP_OFF].bb = brd_get_piece_bb(brd, WHITE_BISHOP);
    val_info[EV_KNIGHT_OFF].bb = brd_get_piece_bb(brd, WHITE_KNIGHT);
    val_info[EV_ROOK_OFF].bb = brd_get_piece_bb(brd, WHITE_ROOK);
    val_info[EV_QUEEN_OFF].bb = brd_get_piece_bb(brd, WHITE_QUEEN);
    val_info[EV_KING_OFF].bb = brd_get_piece_bb(brd, WHITE_KING);
    // evaluate white positions
    score += eval_white_pieces_on_square();

    // populate bitboards for BLACK
    val_info[EV_PAWN_OFF].bb = brd_get_piece_bb(brd, BLACK_PAWN);
    val_info[EV_BISHOP_OFF].bb = brd_get_piece_bb(brd, BLACK_BISHOP);
    val_info[EV_KNIGHT_OFF].bb = brd_get_piece_bb(brd, BLACK_KNIGHT);
    val_info[EV_ROOK_OFF].bb = brd_get_piece_bb(brd, BLACK_ROOK);
    val_info[EV_QUEEN_OFF].bb = brd_get_piece_bb(brd, BLACK_QUEEN);
    val_info[EV_KING_OFF].bb = brd_get_piece_bb(brd, BLACK_KING);
    // evaluate black positions
    score += eval_black_pieces_on_square();

    if (pos_get_side_to_move(pos) == WHITE) {
        return score;
    } else {
        return -score;
    }
}

inline static int32_t eval_white_pieces_on_square() {
    struct value_info *val = val_info;
    int32_t score = 0;

    for (int i = 0; i < NUM_PIECE_ROLES; i++, val++) {
        uint64_t bb = val->bb;

        while (bb != 0) {
            const enum square sq = bb_pop_1st_bit(bb);
            bb = bb_clear_square(bb, sq);

            score += (int32_t)val->val_array[sq];
        }
    }
    return score;
}

inline static int32_t eval_black_pieces_on_square() {
    struct value_info *val = val_info;
    int32_t score = 0;

    for (int i = 0; i < NUM_PIECE_ROLES; i++, val++) {
        uint64_t bb = val->bb;

        while (bb != 0) {
            const enum square sq = bb_pop_1st_bit(bb);
            bb = bb_clear_square(bb, sq);

            const uint8_t mirrored_offset = MIRROR_VALUE[sq];
            score += (int32_t)val->val_array[mirrored_offset];
        }
    }
    return score;
}

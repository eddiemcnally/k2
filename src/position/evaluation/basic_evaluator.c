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
/*! @addtogroup Evaluation
 *
 * @ingroup Evaluation
 * @{
 * @details Contains code for position evaluation
 *
 */

#include "basic_evaluator.h"
#include "bitboard.h"
#include "board.h"
#include "piece.h"
#include "position.h"
#include "square.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static int32_t eval_white_pieces_on_square(const uint64_t bb,
                                           const int8_t pce_table[NUM_SQUARES]);
static int32_t eval_black_pieces_on_square(const uint64_t bb,
                                           const int8_t pce_table[NUM_SQUARES]);
static uint8_t map_square_to_offset(const enum square sq);

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

/**
 * @brief Performs basic evaluation of the position. Limits evaluation 
 * to material and a look-up piece table for piece positions on board.
 * 
 * @param pos       the position
 * @return int32_t  the score
 */
int32_t evaluate_position_basic(const struct position *pos) {

    const struct board *brd = pos_get_board(pos);

    const uint32_t white_material = brd_get_material(brd, WHITE);
    const uint32_t black_material = brd_get_material(brd, BLACK);

    int32_t score = (int32_t)(white_material - black_material);

    // populate bitboards for white
    const uint64_t wp_bb = brd_get_piece_bb(brd, WHITE_PAWN);
    const uint64_t wb_bb = brd_get_piece_bb(brd, WHITE_BISHOP);
    const uint64_t wn_bb = brd_get_piece_bb(brd, WHITE_KNIGHT);
    const uint64_t wr_bb = brd_get_piece_bb(brd, WHITE_ROOK);
    const uint64_t wq_bb = brd_get_piece_bb(brd, WHITE_QUEEN);
    const uint64_t wk_bb = brd_get_piece_bb(brd, WHITE_KING);
    // evaluate white positions
    score += eval_white_pieces_on_square(wp_bb, PAWN_SQ_VALUE);
    score += eval_white_pieces_on_square(wb_bb, BISHOP_SQ_VALUE);
    score += eval_white_pieces_on_square(wn_bb, KNIGHT_SQ_VALUE);
    score += eval_white_pieces_on_square(wr_bb, ROOK_SQ_VALUE);
    score += eval_white_pieces_on_square(wq_bb, QUEEN_SQ_VALUE);
    score += eval_white_pieces_on_square(wk_bb, KING_SQ_VALUE);

    // populate bitboards for black
    const uint64_t bp_bb = brd_get_piece_bb(brd, BLACK_PAWN);
    const uint64_t bb_bb = brd_get_piece_bb(brd, BLACK_BISHOP);
    const uint64_t bn_bb = brd_get_piece_bb(brd, BLACK_KNIGHT);
    const uint64_t br_bb = brd_get_piece_bb(brd, BLACK_ROOK);
    const uint64_t bq_bb = brd_get_piece_bb(brd, BLACK_QUEEN);
    const uint64_t bk_bb = brd_get_piece_bb(brd, BLACK_KING);
    // evaluate black positions
    // NOTE subtraction
    score -= eval_black_pieces_on_square(bp_bb, PAWN_SQ_VALUE);
    score -= eval_black_pieces_on_square(bb_bb, BISHOP_SQ_VALUE);
    score -= eval_black_pieces_on_square(bn_bb, KNIGHT_SQ_VALUE);
    score -= eval_black_pieces_on_square(br_bb, ROOK_SQ_VALUE);
    score -= eval_black_pieces_on_square(bq_bb, QUEEN_SQ_VALUE);
    score -= eval_black_pieces_on_square(bk_bb, KING_SQ_VALUE);

    if (pos_get_side_to_move(pos) == WHITE) {
        return score;
    } else {
        return -score;
    }
}

inline static int32_t
eval_white_pieces_on_square(const uint64_t bitboard,
                            const int8_t pce_table[NUM_SQUARES]) {
    uint64_t bb = bitboard;
    int32_t score = 0;

    while (bb != 0) {
        const enum square sq = bb_pop_1st_bit(bb);
        bb = bb_clear_square(bb, sq);

        const uint8_t offset = map_square_to_offset(sq);
        const int8_t bonus = pce_table[offset];
        score += bonus;
    }
    return score;
}

inline static int32_t
eval_black_pieces_on_square(const uint64_t bitboard,
                            const int8_t pce_table[NUM_SQUARES]) {

    int32_t score = 0;
    uint64_t bb = bitboard;

    while (bb != 0) {
        const enum square sq = bb_pop_1st_bit(bb);
        bb = bb_clear_square(bb, sq);

        const uint8_t offset = map_square_to_offset(sq);
        const uint8_t mirrored_offset = MIRROR_VALUE[offset];
        const int8_t bonus = pce_table[mirrored_offset];
        score += bonus;
    }
    return score;
}

inline static uint8_t map_square_to_offset(const enum square sq) {
    return MIRROR_VALUE[sq];
}

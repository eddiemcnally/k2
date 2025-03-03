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
#include "utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Values for piece square arrays are taken from
// https://www.chessprogramming.org/Simplified_Evaluation_Function
//
// NOTES:
//  -   The above site doesn't have the squares at the correct array offset, so these
//      arrays have been re-sorted to fix that (ie, a1 is offset zero in the array, h8 is 63).
//  -   The arrays below are in terms of WHITE.
//  -   The MIRROR array swaps the arrays into BLACK
//
//

// clang-format off
// elem[0] is a1, elem[1] is b1, etc
static const int8_t PAWN_SQ_VALUE[NUM_SQUARES] = {
    0,      0,      0,      0,      0,      0,      0,      0,  
    5,      10,     10,     -20,    -20,    10,     10,     5,  
    5,      -5,     -10,    0,      0,      -10,    -5,     5,  
    0,      0,      0,      20,     20,     0,      0,      0,  
    5,      5,      10,     25,     25,     10,     5,      5,  
    10,     10,     20,     30,     30,     20,     10,     10, 
    50,     50,     50,     50,     50,     50,     50,     50,  
    0,      0,      0,      0,      0,      0,      0,      0,
};

static const int8_t KNIGHT_SQ_VALUE[NUM_SQUARES] = {
    -50,    -40,    -30,    -30,    -30,    -30,    -40,    -50, 
    -40,    -20,    0,      5,      5,      0,      -20,    -40, 
    -30,    5,      10,     15,     15,     10,     5,      -30, 
    -30,    0,      15,     20,     20,     15,     0,      -30, 
    -30,    5,      15,     20,     20,     15,     5,      -30, 
    -30,    0,      10,     15,     15,     10,     0,      -30, 
    -40,    -20,    0,      0,      0,      0,      -20,    -40, 
    -50,    -40,    -30,    -30,    -30,    -30,    -40,    -50,
};

static const int8_t BISHOP_SQ_VALUE[NUM_SQUARES] = {
    -20,    -10,    -10,    -10,    -10,    -10,    -10,    -20, 
    -10,    5,      0,      0,      0,      0,      5,      -10, 
    -10,    10,     10,     10,     10,     10,     10,     -10, 
    -10,    0,      10,     10,     10,     10,     0,      -10, 
    -10,    5,      5,      10,     10,     5,      5,      -10, 
    -10,    0,      5,      10,     10,     5,      0,      -10, 
    -10,    0,      0,      0,      0,      0,      0,      -10, 
    -20,    -10,    -10,    -10,    -10,    -10,    -10,    -20,
};

static const int8_t ROOK_SQ_VALUE[NUM_SQUARES] = {  
    0,      0,      0,      5,      5,      0,      0,      0,  
    -5,     0,      0,      0,      0,      0,      0,      -5, 
    -5,     0,      0,      0,      0,      0,      0,      -5, 
    -5,     0,      0,      0,      0,      0,      0,      -5,
    -5,     0,      0,      0,      0,      0,      0,      -5, 
    -5,     0,      0,      0,      0,      0,      0,      -5, 
    5,      10,     10,     10,     10,     10,     10,     5,  
    0,      0,      0,      0,      0,      0,      0,      0,
};

static const int8_t QUEEN_SQ_VALUE[NUM_SQUARES] = {
    -20,    -10,    -10,    -5,     -5,     -10,    -10,    -20, 
    -10,    0,      5,      0,      0,      0,      0,      -10, 
    -10,    5,      5,      5,      5,      5,      0,      -10, 
    0,      0,      5,      5,      5,      5,      0,      -5, 
    -5,     0,      5,      5,      5,      5,      0,      -5,  
    -10,    0,      5,      5,      5,      5,      0,      -10, 
    -10,    0,      0,      0,      0,      0,      0,      -10, 
    -20,    -10,    -10,    -5,     -5,     -10,    -10,    -20,
};

static const int8_t KING_SQ_VALUE[NUM_SQUARES] = {
    20,     30,     10,     0,      0,      10,     30,     20,  
    20,     20,     0,      0,      0,      0,      20,     20,  
    -10,    -20,    -20,    -20,    -20,    -20,    -20,    -10, 
    -20,    -30,    -30,    -40,    -40,    -30,    -30,    -20, 
    -30,    -40,    -40,    -50,    -50,    -40,    -40,    -30,
    -30,    -40,    -40,    -50,    -50,    -40,    -40,    -30, 
    -30,    -40,    -40,    -50,    -50,    -40,    -40,    -30,
    -30,    -40,    -40,    -50,    -50,    -40,    -40,    -30,
};

// clang-format off


// ToDo - add game state and swap to this array
//
// static const int8_t KING_SQ_ENDGAME_VALUE [] = {
//      -50, -30, -30, -30, -30, -30, -30, -50,
//      -30, -30,  0,   0,   0,   0,  -30, -30,
//      -30, -10,  20,  30,  30,  20, -10, -30,
//      -30, -10,  30,  40,  40,  30, -10, -30,
//      -30, -10,  30,  40,  40,  30, -10, -30,
//      -30, -10,  20,  30,  30,  20, -10, -30,
//      -30, -20, -10,  0,   0,  -10, -20, -30,
//      -50, -40, -30, -20, -20, -30, -40, -50,
// };

/**
 * @brief Performs basic evaluation of the board. Limits evaluation 
 * to material and a look-up piece table for piece positions on board.
 * 
 * @param brd               the board
 * @param side_to_move      the side to move
 * @return int32_t          the score
 */
int32_t evaluate_position_basic(const struct board * const brd, const enum colour side_to_move) {

    const struct material m = brd_get_material(brd);

    int32_t score = m.white - m.black;

    uint64_t pce_bb = brd_get_board_bb(brd);
    while (pce_bb != 0) {
        const enum square sq = bb_pop_1st_bit_and_clear(&pce_bb);
        enum piece pce;
        brd_try_get_piece_on_square(brd, sq, &pce);
        
        switch(pce){
        case WHITE_PAWN:
            score += PAWN_SQ_VALUE[sq];
            break;
        case WHITE_BISHOP:
            score += BISHOP_SQ_VALUE[sq];
            break;
        case WHITE_KNIGHT:
            score += KNIGHT_SQ_VALUE[sq];
            break;
        case WHITE_ROOK:
            score += ROOK_SQ_VALUE[sq];
            break;
        case WHITE_QUEEN:
            score += QUEEN_SQ_VALUE[sq];
            break;
        case WHITE_KING:
            score += KING_SQ_VALUE[sq];
            break;
        case BLACK_PAWN:
            score -= PAWN_SQ_VALUE[63 - sq];
            break;
        case BLACK_BISHOP:
            score -= BISHOP_SQ_VALUE[63 - sq];
            break;
        case BLACK_KNIGHT:
            score -= KNIGHT_SQ_VALUE[63 - sq];
            break;
        case BLACK_ROOK:
            score -= ROOK_SQ_VALUE[63 - sq];
            break;
        case BLACK_QUEEN:
            score -= QUEEN_SQ_VALUE[63 - sq];
            break;
        case BLACK_KING:
            score -= KING_SQ_VALUE[63 - sq];
            break; 
        default:
        print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Invalid piece");
        break;
        }
    } // while

    if (side_to_move == WHITE) {
        return score;
    } else {
        return -score;
    }
}




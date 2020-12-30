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

/*! @addtogroup Search
 *
 * @ingroup Alpha-Beta
 * @{
 * @details Implements the Alpha-Beta search function
 *
 */

#include "alpha_beta.h"
#include "basic_evaluator.h"
#include "move_gen.h"
#include "move_list.h"
#include "quiesence.h"
#include "search.h"
#include "transposition_table.h"
#include <assert.h>
#include <limits.h>

int32_t alpha_beta_search(int32_t alpha, int32_t beta, uint8_t depth, struct position *pos,
                          struct search_data *search_info) {

    assert(validate_position(pos));

    printf("AlphaBeta depth=%d\n", depth);

    if (depth == 0) {
        printf("AlphaBeta depth ZERO, calling quiesence\n");

        return quiescence(pos, search_info, alpha, beta);
    }

    if (pos_get_ply(pos) > MAX_SEARCH_DEPTH - 1) {
        return evaluate_position_basic(pos);
    }

    int32_t score = NEG_INFINITY;
    struct move best_move;
    bool legal_moves_available = false;
    const int32_t entry_alpha = alpha;

    // todo:
    // check repetition
    // check 50 move rule
    // check max depth
    // check search_info->node count > MAX COUNT

    struct move_list mv_list = mvl_initialise();

    mv_gen_all_moves(pos, &mv_list);

    printf("AlphaBeta depth=%d, num moves generated=%d\n", depth, mv_list.move_count);

    for (int i = 0; i < mv_list.move_count; i++) {

        struct move mv = mv_list.move_list[i];
        best_move = mv;

        printf("AlphaBeta depth=%d, processing move %s\n", depth, move_print(mv));

        enum move_legality legality = pos_make_move(pos, mv);
        if (legality != LEGAL_MOVE) {
            pos_take_move(pos);
            continue;
        }

        legal_moves_available = true;

        printf("AlphaBeta depth=%d, calling alphabeta\n", depth);

        // NOTE alpha/beta swapped, negate the result
        score = -alpha_beta_search(-beta, -alpha, (depth - 1), pos, search_info);
        pos_take_move(pos);

        if (score > alpha) {
            if (score >= beta) {
                printf("AlphaBeta depth=%d, score > beta\n", depth);
                return beta;
            }
            alpha = score;
            best_move = mv;
            printf("AlphaBeta depth=%d, setting BestMove = %s\n", depth, move_print(mv));
        }
    }

    if (legal_moves_available == false) {
        // todo - handle this
    }

    if (alpha != entry_alpha) {
        printf("AlphaBeta depth=%d, adding to TT, move=%s\n", depth, move_print(best_move));
        // TODO
        //tt_add(pos_get_hash(pos), best_move, depth);
    }

    return alpha;
}

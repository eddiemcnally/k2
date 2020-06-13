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
 * @ingroup Quiesence
 * @{
 * @details Implements the Quiesence search function
 *
 */

#include "quiesence.h"
#include "basic_evaluator.h"
#include "move_gen.h"
#include "move_list.h"

static struct move_list mvl;
static bool is_mvl_init = false;

int32_t quiescence(struct position *pos, struct search_data *search, int32_t alpha, int32_t beta) {
    if (is_mvl_init == false) {
        is_mvl_init = true;
        mvl_initialise(&mvl);
    }
    mvl_reset(&mvl);

    // todo
    // - check for time limit every 'n' moves
    // - check on move repetitpon and 50-move counter

    if (pos_get_ply(pos) > MAX_SEARCH_DEPTH - 1) {
        return evaluate_position_basic(pos);
    }

    // stand pat
    int32_t stand_pat_score = evaluate_position_basic(pos);
    if (stand_pat_score >= beta) {
        search->stand_pat_cutoff++;
        return beta;
    }
    if (stand_pat_score > alpha) {
        search->stand_pat_improvement++;
        alpha = stand_pat_score;
    }

    mv_gen_only_capture_moves(pos, &mvl);

    uint16_t num_moves = mvl.move_count;
    for (uint16_t i = 0; i < num_moves; i++) {
        struct move mv = mvl.move_list[i];
        enum move_legality legality = pos_make_move(pos, mv);
        if (legality == LEGAL_MOVE) {
            // note: alpha/beta are swapped, and sign is reversed
            int32_t score = -quiescence(pos, search, -beta, -alpha);
            pos_take_move(pos);

            if (search->search_stopped == true) {
                // timed out
                return 0;
            }

            if (score > alpha) {
                if (score >= beta) {
                    return beta;
                }

                alpha = score;
            }
        }
    }
    return alpha;
}

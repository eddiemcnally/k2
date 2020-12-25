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
#include "move_gen.h"
#include "move_list.h"
#include "quiesence.h"
#include "search.h"
#include <limits.h>

#define MAX_PV_MOVES 4098

// PV table
struct pv_line {
    uint16_t num_moves;              // Number of moves in the line.
    struct move moves[MAX_PV_MOVES]; // The line.
};

int32_t alpha_beta_search(int32_t alpha, int32_t beta, uint8_t depth, struct position *pos,
                          struct search_data *search_info) {

    if (depth == 0) {
        return quiescence(pos, search_info, alpha, beta);
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

    for (int i = 0; i < mv_list.move_count; i++) {

        const struct move mv = mv_list.move_list[i];
        enum move_legality legality = pos_make_move(pos, mv);
        if (legality != LEGAL_MOVE) {
            pos_take_move(pos);
            continue;
        }

        legal_moves_available = true;

        // NOTE alpha/beta swapped
        score = -alpha_beta_search(-beta, -alpha, depth - 1, pos, search_info);
        pos_take_move(pos);

        if (score > alpha) {
            if (score >= beta) {
                return beta;
            }
            alpha = score;
            best_move = mv;
        }
    }

    if (legal_moves_available == false) {
        // todo - handle this
    }

    if (alpha != entry_alpha) {
        // save PV move
    }

    return alpha;
}

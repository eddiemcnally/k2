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
 * @ingroup Search
 * @{
 * @details Functions associated with position search
 *
 */

#include "search.h"
#include "alpha_beta.h"
#include "board.h"
#include "move.h"
#include "move_gen.h"
#include "move_list.h"
#include "transposition_table.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>

static struct pv_line get_pv_line(uint8_t depth, struct position *pos);
static bool move_exists(struct position *pos, const struct move mv);

#define TT_SIZE_IN_BYTES (200000000)

struct pv_line {
    uint16_t num_moves;
    struct move line[MAX_SEARCH_DEPTH];
};

void search_position(struct position *pos, struct search_data *search_info) {

    assert(validate_position(pos));

    tt_create(TT_SIZE_IN_BYTES);

    int32_t best_score = INT_MIN;

    for (uint8_t depth = 1; depth <= search_info->search_depth; depth++) {

        printf("Search depth=%d\n", depth);

        best_score = alpha_beta_search(NEG_INFINITY, INFINITY, depth, pos, search_info);

        const struct pv_line line = get_pv_line(depth, pos);

        const struct move best_move = line.line[0];

        printf("SEARCH : depth=%d, PV Line size=%d\n", depth, line.num_moves);
        printf("SEARCH : Best move %s\n", move_print(best_move));
    }

    tt_dispose();
}

static struct pv_line get_pv_line(uint8_t depth, struct position *pos) {

    struct pv_line retval;

    struct move mv;
    bool found = tt_probe_position(pos_get_hash(pos), &mv);
    uint8_t i = 0;

    while (found && i < depth) {
        if (move_exists(pos, mv)) {
            pos_make_move(pos, mv);
            retval.line[i] = mv;
            i++;
        } else {
            break;
        }
        found = tt_probe_position(pos_get_hash(pos), &mv);
    }

    while (pos_get_ply(pos) > 0) {
        pos_take_move(pos);
    }

    return retval;
}

static bool move_exists(struct position *pos, const struct move mv) {
    struct move_list mvl = mvl_initialise();

    mv_gen_all_moves(pos, &mvl);

    for (int i = 0; i < mvl.move_count; i++) {
        enum move_legality legal = pos_make_move(pos, mvl.move_list[i]);
        if (legal != LEGAL_MOVE) {
            continue;
        }
        pos_take_move(pos);

        if (move_compare(mvl.move_list[i], mv)) {
            return true;
        }
    }
    return false;
}

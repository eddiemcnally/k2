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
#include "attack_checker.h"
#include "basic_evaluator.h"
#include "board.h"
#include "move.h"
#include "move_gen.h"
#include "move_list.h"
#include "transposition_table.h"
#include "utils.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>

static struct pv_line get_pv_line(uint8_t depth, struct position *pos);
static bool move_exists(struct position *pos, const uint64_t mv);
static int32_t quiescence(struct position *const pos, struct search_data *const search, int32_t alpha, int32_t beta);
static int32_t alpha_beta_search(const int32_t alpha, int32_t beta, uint8_t depth, struct position *const pos,
                                 struct search_data *const search_info);

#define TT_SIZE_IN_BYTES (200000000)

struct pv_line {
    uint16_t num_moves;
    uint64_t line[MAX_SEARCH_DEPTH];
};

void search_position(struct position *pos, struct search_data *search_info) {

    assert(validate_position(pos));

    tt_create(TT_SIZE_IN_BYTES);

    int32_t best_score = INT_MIN;

    for (uint8_t depth = 1; depth <= search_info->search_depth; depth++) {

        printf("Search depth=%d\n", depth);

        best_score = alpha_beta_search(NEG_INFINITY, INFINITY, depth, pos, search_info);

        const struct pv_line line = get_pv_line(depth, pos);

        const uint64_t best_move = line.line[0];

        printf("SEARCH : depth=%d, PV Line size=%d\n", depth, line.num_moves);
        printf("SEARCH : Best move %s\n", move_print(best_move));
    }

    tt_dispose();
}

static struct pv_line get_pv_line(uint8_t depth, struct position *pos) {

    struct pv_line retval;

    uint64_t mv;
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

static bool move_exists(struct position *pos, const uint64_t mv) {
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

static int32_t alpha_beta_search(int32_t alpha, int32_t beta, uint8_t depth, struct position *pos,
                                 struct search_data *search_info) {

    assert(validate_position(pos));

    //printf("AlphaBeta depth=%d\n", depth);

    if (depth == 0) {
        //printf("AlphaBeta depth ZERO, calling quiesence\n");

        return quiescence(pos, search_info, alpha, beta);
    }

    const struct board *brd = pos_get_board(pos);
    const enum colour side_to_move = pos_get_side_to_move(pos);

    if (pos_get_ply(pos) > MAX_SEARCH_DEPTH - 1) {
        return evaluate_position_basic(brd, side_to_move);
    }

    // todo:
    // check repetition
    // check 50 move rule
    // check max depth

    const enum square king_sq = side_to_move == WHITE ? brd_get_white_king_square(brd) : brd_get_black_king_square(brd);

    const bool is_in_check = att_chk_is_sq_attacked(pos, king_sq, side_to_move);
    if (is_in_check) {
        depth++;
    }

    int32_t score = NEG_INFINITY;
    uint64_t best_move;
    bool legal_moves_available = false;
    const int32_t entry_alpha = alpha;
    const uint64_t pos_hash = pos_get_hash(pos);

    // Generate all moves
    struct move_list mv_list = mvl_initialise();
    mv_gen_all_moves(pos, &mv_list);

    // check if position already exists
    uint64_t tt_move = {0};
    const bool found_in_tt = tt_probe_position(pos_hash, &tt_move);
    if (found_in_tt) {
        bool mv_found = false;
        for (int i = 0; i < mv_list.move_count; i++) {
            uint64_t new_mv = mv_list.move_list[i];
            if (move_compare(tt_move, new_mv)) {
                new_mv = move_set_score(new_mv, 2000000);
                mv_found = true;
                break;
            }
        }
        REQUIRE(mv_found, "Found position, but can't find move");
    }

    //printf("AlphaBeta depth=%d, num moves generated=%d\n", depth, mv_list.move_count);

    for (uint32_t i = 0; i < mv_list.move_count; i++) {

        mvl_move_highest_score_to_start_of_slice(&mv_list, i);

        uint64_t mv = mv_list.move_list[i];
        best_move = mv;

        enum move_legality legality = pos_make_move(pos, mv);
        if (legality != LEGAL_MOVE) {
            continue;
        }

        legal_moves_available = true;

        //printf("AlphaBeta depth=%d, calling alphabeta\n", depth);

        // NOTE alpha/beta swapped, negate the result
        score = -alpha_beta_search(-beta, -alpha, (depth - 1), pos, search_info);
        pos_take_move(pos);

        if (score > alpha) {
            if (score >= beta) {
                //printf("AlphaBeta depth=%d, score > beta\n", depth);
                return beta;
            }
            alpha = score;
            best_move = mv;
            //printf("AlphaBeta depth=%d, setting BestMove = %s\n", depth, move_print(mv));
        }
    }

    if (legal_moves_available == false) {
        // todo - handle this
    }

    if (alpha != entry_alpha) {
        //printf("AlphaBeta depth=%d, adding to TT, move=%s\n", depth, move_print(best_move));

        tt_add(pos_hash, best_move, depth, alpha, NODE_ALPHA);
    }

    return alpha;
}

static int32_t quiescence(struct position *pos, struct search_data *search, int32_t alpha, int32_t beta) {

    const int32_t orig_alpha = alpha;

    assert(validate_position(pos));

    struct move_list mvl = mvl_initialise();
    mvl_reset(&mvl);

    // todo
    // - check for time limit every 'n' moves
    // - check on move repetiton and 50-move counter
    const struct board *brd = pos_get_board(pos);
    const enum colour colour = pos_get_side_to_move(pos);
    if (pos_get_ply(pos) > MAX_SEARCH_DEPTH - 1) {
        return evaluate_position_basic(brd, colour);
    }

    // stand pat
    int32_t stand_pat_score = evaluate_position_basic(brd, colour);
    if (stand_pat_score >= beta) {
        search->stand_pat_cutoff++;
        return beta;
    }
    if (stand_pat_score > alpha) {
        search->stand_pat_improvement++;
        alpha = stand_pat_score;
    }

    mv_gen_only_capture_moves(pos, &mvl);

    uint64_t best_move;

    uint16_t num_moves = mvl.move_count;
    for (uint16_t i = 0; i < num_moves; i++) {
        uint64_t mv = mvl.move_list[i];

        //printf("QUIESCENCE : processing move %s\n", move_print(mv));

        enum move_legality legality = pos_make_move(pos, mv);
        if (legality != LEGAL_MOVE) {
            pos_take_move(pos);
            continue;
        }

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
            best_move = mv;
        }
    }

    if (alpha != orig_alpha) {
        // TODO
        // tt_add(pos_get_hash(pos), best_move, search->search_depth);
    }

    return alpha;
}

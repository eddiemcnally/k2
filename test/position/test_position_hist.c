/*  MIT License
 *
 *  Copyright (c) 2019 Eddie McNally
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

#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "test_position_hist.h"
#include "position.h"
#include "position_hist.h"
#include <assert.h>
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

struct hist_data {
    struct move mv;
    struct en_pass_active en_passant;
    uint64_t hashkey;
    struct cast_perm_container castle_perm_container;
    uint8_t fifty_move_counter;
};

void test_move_history_push_multiple_moves_used_slots_as_expected(
    void **state) {

#define INITIAL_FEN_BLACK_TO_MOVE                                              \
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1\n"

    struct position *pos = pos_create();
    pos_initialise(INITIAL_FEN_BLACK_TO_MOVE, pos);

    struct move_hist *mh = move_hist_init();

    const uint16_t NUM_TO_TEST = MAX_GAME_MOVES - 1;

    for (int i = 0; i < NUM_TO_TEST; i++) {
        // set up some test data
        struct move mv = move_encode_quiet(a1, a3);
        struct en_pass_active en_pass = {.is_active = false};
        struct cast_perm_container cp;
        cast_perm_set_permission(CP_WK, &cp, true);

        move_hist_push(mh, mv, (uint8_t)i, en_pass, (uint64_t)(i * i), cp,
                       pos_get_board(pos));

        assert_true(move_hist_get_num(mh) == i + 1);
    }
    assert_true(move_hist_get_num(mh) == NUM_TO_TEST);

    move_hist_release_memory(mh);
}

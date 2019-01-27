/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "test_position.h"
#include "board.h"
#include "position.h"
#include <cmocka.h>

struct mv_from_to {
    enum square from_sq;
    enum square to_sq;
};

void test_position_get_set_castle_permissions(void** state)
{
    struct cast_perm cp = { .val = 0 };
    cast_perm_set_WK(&cp, true);
    struct position* pos = pos_create();

    pos_set_cast_perm(pos, cp);
    struct cast_perm retrieved_cp = pos_get_cast_perm(pos);
    assert_true(cast_compare_perms(cp, retrieved_cp));

    cast_perm_set_WQ(&cp, true);
    pos_set_cast_perm(pos, cp);
    retrieved_cp = pos_get_cast_perm(pos);
    assert_true(cast_compare_perms(cp, retrieved_cp));

    cast_perm_set_BK(&cp, true);
    pos_set_cast_perm(pos, cp);
    retrieved_cp = pos_get_cast_perm(pos);
    assert_true(cast_compare_perms(cp, retrieved_cp));

    cast_perm_set_BQ(&cp, true);
    pos_set_cast_perm(pos, cp);
    retrieved_cp = pos_get_cast_perm(pos);
    assert_true(cast_compare_perms(cp, retrieved_cp));
}

void test_position_compare(void** state)
{
    const char* FEN = "6Br/R3B3/5NPn/PNpn1k1r/3P4/q2pQ3/bR6/4bK2 w - - 0 1\n";

    struct position* pos1 = pos_create();
    pos_initialise(FEN, pos1);
    struct board* brd1 = pos_get_board(pos1);

    struct position* pos2 = pos_create();
    pos_initialise(FEN, pos2);

    assert_true(pos_compare(pos1, pos2));

    brd_add_piece(brd1, WPAWN, a1);
    assert_false(pos_compare(pos1, pos2));

    brd_remove_piece(brd1, WPAWN, a1);
    assert_true(pos_compare(pos1, pos2));
}

void test_position_white_double_first_move(void** state)
{
    struct mv_from_to moves[8] = {
        { .from_sq = a2, .to_sq = a4 },
        { .from_sq = b2, .to_sq = b4 },
        { .from_sq = c2, .to_sq = c4 },
        { .from_sq = d2, .to_sq = d4 },
        { .from_sq = e2, .to_sq = e4 },
        { .from_sq = f2, .to_sq = f4 },
        { .from_sq = g2, .to_sq = g4 },
        { .from_sq = h2, .to_sq = h4 }
    };

    struct move quiet_move = move_encode_quiet(a2, a3);
    
    for (int i = 0; i < 8; i++) {
        struct position* pos = pos_create();
        pos_initialise(INITIAL_FEN, pos);

        struct move mv = move_encode_pawn_double_first(moves[i].from_sq, moves[i].to_sq);

        // baseline, not set
        enum square enp_sq;
        bool found = pos_try_get_en_pass_sq(pos, &enp_sq);
        assert_false(found);

        // make move and check en passant square
        pos_try_make_move(pos, mv);
        found = pos_try_get_en_pass_sq(pos, &enp_sq);
        assert_true(found);

        const enum square expected_enp_sq = sq_get_square_plus_1_rank(moves[i].from_sq);
        assert_true(expected_enp_sq == enp_sq);

        // make a normal move, verify en passant square no longer active
        pos_try_make_move(pos, quiet_move);
        found = pos_try_get_en_pass_sq(pos, &enp_sq);
        assert_false(found);
        
        pos_destroy(pos);
    }
}



void test_position_black_double_first_move(void** state)
{
    struct mv_from_to moves[8] = {
        { .from_sq = a7, .to_sq = a5 },
        { .from_sq = b7, .to_sq = b5 },
        { .from_sq = c7, .to_sq = c5 },
        { .from_sq = d7, .to_sq = d5 },
        { .from_sq = e7, .to_sq = e5 },
        { .from_sq = f7, .to_sq = f5 },
        { .from_sq = g7, .to_sq = g5 },
        { .from_sq = h7, .to_sq = h5 }
    };

    struct move quiet_move = move_encode_quiet(a7, a6);
    
    for (int i = 0; i < 8; i++) {
        struct position* pos = pos_create();
        pos_initialise(INITIAL_FEN, pos);

        struct move mv = move_encode_pawn_double_first(moves[i].from_sq, moves[i].to_sq);

        // baseline, not set
        enum square enp_sq;
        bool found = pos_try_get_en_pass_sq(pos, &enp_sq);
        assert_false(found);

        // make move and check en passant square
        pos_try_make_move(pos, mv);
        found = pos_try_get_en_pass_sq(pos, &enp_sq);
        assert_true(found);

        const enum square expected_enp_sq = sq_get_square_minus_1_rank(moves[i].from_sq);
        assert_true(expected_enp_sq == enp_sq);

        // make a normal move, verify en passant square no longer active
        pos_try_make_move(pos, quiet_move);
        found = pos_try_get_en_pass_sq(pos, &enp_sq);
        assert_false(found);
        
        pos_destroy(pos);
    }
}


















void test_position_brd_is_sq_occupied(void** state)
{
    const char* FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/1bNnrQ2 w - - 0 1\n";

    struct position* pos = pos_create();
    pos_initialise(FEN, pos);

    struct board* brd = pos_get_board(pos);

    assert_false(brd_is_sq_occupied(brd, a1));
    assert_false(brd_is_sq_occupied(brd, a2));
    assert_false(brd_is_sq_occupied(brd, a3));
    assert_true(brd_is_sq_occupied(brd, a4));
    assert_false(brd_is_sq_occupied(brd, a5));
    assert_false(brd_is_sq_occupied(brd, a6));
    assert_true(brd_is_sq_occupied(brd, a7));
    assert_false(brd_is_sq_occupied(brd, a8));

    assert_true(brd_is_sq_occupied(brd, b1));
    assert_true(brd_is_sq_occupied(brd, b2));
    assert_false(brd_is_sq_occupied(brd, b3));
    assert_true(brd_is_sq_occupied(brd, b4));
    assert_false(brd_is_sq_occupied(brd, b5));
    assert_true(brd_is_sq_occupied(brd, b6));
    assert_true(brd_is_sq_occupied(brd, b7));
    assert_true(brd_is_sq_occupied(brd, b8));

    assert_true(brd_is_sq_occupied(brd, c1));
    assert_false(brd_is_sq_occupied(brd, c2));
    assert_true(brd_is_sq_occupied(brd, c3));
    assert_true(brd_is_sq_occupied(brd, c4));
    assert_false(brd_is_sq_occupied(brd, c5));
    assert_false(brd_is_sq_occupied(brd, c6));
    assert_false(brd_is_sq_occupied(brd, c7));
    assert_false(brd_is_sq_occupied(brd, c8));

    assert_true(brd_is_sq_occupied(brd, d1));
    assert_false(brd_is_sq_occupied(brd, d2));
    assert_true(brd_is_sq_occupied(brd, d3));
    assert_false(brd_is_sq_occupied(brd, d4));
    assert_true(brd_is_sq_occupied(brd, d5));
    assert_false(brd_is_sq_occupied(brd, d6));
    assert_false(brd_is_sq_occupied(brd, d7));
    assert_true(brd_is_sq_occupied(brd, d8));

    assert_true(brd_is_sq_occupied(brd, e1));
    assert_true(brd_is_sq_occupied(brd, e2));
    assert_false(brd_is_sq_occupied(brd, e3));
    assert_true(brd_is_sq_occupied(brd, e4));
    assert_false(brd_is_sq_occupied(brd, e5));
    assert_false(brd_is_sq_occupied(brd, e6));
    assert_false(brd_is_sq_occupied(brd, e7));
    assert_true(brd_is_sq_occupied(brd, e8));

    assert_true(brd_is_sq_occupied(brd, f1));
    assert_false(brd_is_sq_occupied(brd, f2));
    assert_true(brd_is_sq_occupied(brd, f3));
    assert_true(brd_is_sq_occupied(brd, f4));
    assert_true(brd_is_sq_occupied(brd, f5));
    assert_true(brd_is_sq_occupied(brd, f6));
    assert_false(brd_is_sq_occupied(brd, f7));
    assert_true(brd_is_sq_occupied(brd, f8));

    assert_false(brd_is_sq_occupied(brd, g1));
    assert_true(brd_is_sq_occupied(brd, g2));
    assert_true(brd_is_sq_occupied(brd, g3));
    assert_true(brd_is_sq_occupied(brd, g4));
    assert_true(brd_is_sq_occupied(brd, g5));
    assert_false(brd_is_sq_occupied(brd, g6));
    assert_false(brd_is_sq_occupied(brd, g7));
    assert_false(brd_is_sq_occupied(brd, g8));

    assert_false(brd_is_sq_occupied(brd, h1));
    assert_true(brd_is_sq_occupied(brd, h2));
    assert_true(brd_is_sq_occupied(brd, h3));
    assert_false(brd_is_sq_occupied(brd, h4));
    assert_false(brd_is_sq_occupied(brd, h5));
    assert_true(brd_is_sq_occupied(brd, h6));
    assert_false(brd_is_sq_occupied(brd, h7));
    assert_false(brd_is_sq_occupied(brd, h8));
}

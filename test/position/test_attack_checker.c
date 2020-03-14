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

#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "test_attack_checker.h"
#include "attack_checker.h"
#include "position.h"
#include <cmocka.h>
#include <stdint.h>

void test_att_chk_is_white_pawn_attacking(void **state) {
    const char *fen = "3K4/3p2P1/pP1pP2p/1p4PP/1p1P1P2/2P2ppk/8/8 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[14] = {b4, d4, c5, e5, g5, f6, h6,
                                   g6, a7, c7, d7, f7, f8, h8};
    for (int i = 0; i < 14; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], WHITE));
    }

    enum square non_attacked_sq[4] = {c4, d5, b7, g8};
    for (int i = 0; i < 4; i++) {
        assert_false(att_chk_is_sq_attacked(brd, non_attacked_sq[i], WHITE));
    }
}

void test_att_chk_is_black_pawn_attacking(void **state) {
    const char *fen = "k7/3p2P1/pP1pP2p/1p4PP/1p1P1P2/2P2pp1/8/K7 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[14] = {a3, c3, a4, c4, b5, c5, c6,
                                   e5, e6, e2, f2, g2, h2, g5};
    for (int i = 0; i < 14; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], BLACK));
    }

    enum square non_attacked_sq[4] = {a5, b3, d5, h5};
    for (int i = 0; i < 4; i++) {
        assert_false(att_chk_is_sq_attacked(brd, non_attacked_sq[i], BLACK));
    }
}

void test_att_chk_is_white_knight_attacking(void **state) {
    const char *fen = "6k1/1n6/8/8/2N5/5n2/6N1/K7 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[12] = {e1, e3, f4, h4, b6, a5,
                                   a3, b2, d2, e3, e5, d6};
    for (int i = 0; i < 12; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], WHITE));
    }
}

void test_att_chk_is_black_knight_attacking(void **state) {
    const char *fen = "6k1/1n6/8/8/2N5/5n2/6N1/K7 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[12] = {d8, d6, c5, a5, h2, g1,
                                   e1, d2, d4, e5, g5, h4};
    for (int i = 0; i < 12; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], BLACK));
    }
}

void test_att_chk_is_white_king_attacking(void **state) {
    const char *fen = "6k1/8/8/8/8/3K4/8/8 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[8] = {c2, d2, e2, c3, e3, c4, d4, e4};
    for (int i = 0; i < 8; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], WHITE));
    }
}

void test_att_chk_is_black_king_attacking(void **state) {
    const char *fen = "6k1/8/8/8/8/3K4/8/8 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[5] = {f7, g7, h7, h8, f8};
    for (int i = 0; i < 5; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], BLACK));
    }
}

void test_att_chk_is_white_horizontal_and_vertical_attacking(void **state) {
    const char *fen =
        "K3k3/2B1p2n/4pP2/Nq3r2/1P1Ppp1R/8/2P1P2p/R1b1n2Q w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[17] = {b1, c1, g1, f1, e1, a2, a3, a4, h2,
                                   h3, g2, f3, g4, f4, h5, h6, h7};
    for (int i = 0; i < 17; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], WHITE));
    }

    enum square not_attacked_sq[4] = {d1, a6, h8, d5};
    for (int i = 0; i < 4; i++) {
        assert_false(att_chk_is_sq_attacked(brd, not_attacked_sq[i], WHITE));
    }
}

void test_att_chk_is_black_horizontal_and_vertical_attacking(void **state) {
    const char *fen = "r2b4/r4PPP/4RP2/p1K2k1q/5Pp1/P3p2N/1p1b4/Q3R3 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[19] = {a5, a6, b8, c8, d8, b7, c7, d7, e7, f7,
                                   h4, h3, g4, g5, f5, g6, f7, h6, h7};
    for (int i = 0; i < 19; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], BLACK));
    }

    enum square not_attacked_sq[5] = {a4, e8, g7, h8, h2};
    for (int i = 0; i < 5; i++) {
        assert_false(att_chk_is_sq_attacked(brd, not_attacked_sq[i], BLACK));
    }
}

void test_att_chk_is_white_diagonal_attacking(void **state) {
    const char *fen = "3B1q2/2n5/5Pp1/1k1p3P/b5B1/n2p2Q1/NNpp1b2/2K5 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[19] = {c7, e7, f6, f5, e6, d7, c8, h3, h5, f3,
                                   e2, d1, h4, f2, h2, f4, e5, d6, c7};
    for (int i = 0; i < 19; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], WHITE));
    }

    enum square not_attacked_sq[4] = {b8, b6, b7, e1};
    for (int i = 0; i < 4; i++) {
        assert_false(att_chk_is_sq_attacked(brd, not_attacked_sq[i], WHITE));
    }
}

void test_att_chk_is_black_diagonal_attacking(void **state) {
    const char *fen = "k2B1q2/5p2/5Pp1/7P/b2p2B1/6Q1/NNK2b2/8 w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(fen, pos);
    struct board *brd = pos_get_board(pos);

    enum square attacked_sq[15] = {b3, c2, b5, c6, d7, g1, e3, d4,
                                   g7, h6, e7, d6, c5, b4, a3};
    for (int i = 0; i < 15; i++) {
        assert_true(att_chk_is_sq_attacked(brd, attacked_sq[i], BLACK));
    }

    enum square not_attacked_sq[2] = {d1, h4};
    for (int i = 0; i < 2; i++) {
        assert_false(att_chk_is_sq_attacked(brd, not_attacked_sq[i], BLACK));
    }
}

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
#include <cmocka.h>

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

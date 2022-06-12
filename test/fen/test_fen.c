/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
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

#include "test_fen.h"
#include "fen.h"
#include "piece.h"
#include "position.h"
#include "square.h"
#include <cmocka.h>

struct sq_pce {
    struct piece *piece;
    bool has_piece;
};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"

void test_fen_pieces_init_position(void **state) {
    struct parsed_fen *brd = fen_parse(INITIAL_FEN);

    // setup test data for comparison
    struct sq_pce data[NUM_SQUARES];
    for (int i = a1; i < h8; i++) {
        data[i].has_piece = false;
        data[i].piece = NULL;
    }

    data[a1].has_piece = true;
    data[a1].piece = pce_get_white_rook();
    data[b1].has_piece = true;
    data[b1].piece = pce_get_white_knight();
    data[c1].has_piece = true;
    data[c1].piece = pce_get_white_bishop();
    data[d1].has_piece = true;
    data[d1].piece = pce_get_white_queen();
    data[e1].has_piece = true;
    data[e1].piece = pce_get_white_king();
    data[f1].has_piece = true;
    data[f1].piece = pce_get_white_bishop();
    data[g1].has_piece = true;
    data[g1].piece = pce_get_white_knight();
    data[h1].has_piece = true;
    data[h1].piece = pce_get_white_rook();
    for (int i = a2; i <= h2; i++) {
        data[i].has_piece = true;
        data[i].piece = pce_get_white_pawn();
    }

    data[a8].has_piece = true;
    data[a8].piece = pce_get_black_rook();
    data[b8].has_piece = true;
    data[b8].piece = pce_get_black_knight();
    data[c8].has_piece = true;
    data[c8].piece = pce_get_black_bishop();
    data[d8].has_piece = true;
    data[d8].piece = pce_get_black_queen();
    data[e8].has_piece = true;
    data[e8].piece = pce_get_black_king();
    data[f8].has_piece = true;
    data[f8].piece = pce_get_black_bishop();
    data[g8].has_piece = true;
    data[g8].piece = pce_get_black_knight();
    data[h8].has_piece = true;
    data[h8].piece = pce_get_black_rook();
    for (int i = a7; i <= h7; i++) {
        data[i].has_piece = true;
        data[i].piece = pce_get_black_pawn();
    }

    bool found = false;
    struct piece *pce;
    for (int s = a1; s <= h8; s++) {
        struct sq_pce d = data[s];

        bool b = fen_try_get_piece_on_sq(brd, (enum square)s, &pce);

        if (b == true) {
            assert_true(pce == d.piece);
        }
    }

    // all other squares are empty
    for (int r = RANK_3; r <= RANK_6; r++) {
        for (int f = FILE_A; f <= FILE_H; f++) {
            enum square sq = sq_gen_from_rank_file((enum rank)r, (enum file)f);

            found = fen_try_get_piece_on_sq(brd, sq, &pce);
            assert_false(found);
        }
    }
}

void test_fen_pieces_random_position_1(void **state) {
    const char *RANDOM_FEN_1 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/"
                               "R4RK1 w Qkq - 1 4\n";
    struct parsed_fen *brd = fen_parse(RANDOM_FEN_1);

    // setup test data for comparison
    struct sq_pce data[NUM_SQUARES];
    for (int i = a1; i < h8; i++) {
        data[a1].has_piece = false;
    }

    // RANK 1
    data[a1].has_piece = true;
    data[a1].piece = pce_get_white_rook();

    data[f1].has_piece = true;
    data[f1].piece = pce_get_white_rook();
    data[g1].has_piece = true;
    data[g1].piece = pce_get_white_king();

    // RANK 2
    data[b2].has_piece = true;
    data[b2].piece = pce_get_white_bishop();
    data[d2].has_piece = true;
    data[d2].piece = pce_get_white_pawn();
    data[e2].has_piece = true;
    data[e2].piece = pce_get_white_queen();
    data[f2].has_piece = true;
    data[f2].piece = pce_get_white_pawn();
    data[h2].has_piece = true;
    data[h2].piece = pce_get_white_pawn();

    // RANK 3
    data[a3].has_piece = true;
    data[a3].piece = pce_get_white_pawn();
    data[b3].has_piece = true;
    data[b3].piece = pce_get_white_pawn();
    data[c3].has_piece = true;
    data[c3].piece = pce_get_white_knight();
    data[d3].has_piece = true;
    data[d3].piece = pce_get_white_bishop();
    data[f3].has_piece = true;
    data[f3].piece = pce_get_white_knight();
    data[g3].has_piece = true;
    data[g3].piece = pce_get_white_pawn();
    data[h3].has_piece = true;
    data[h3].piece = pce_get_black_bishop();

    // RANK 4
    data[b4].has_piece = true;
    data[b4].piece = pce_get_black_bishop();
    data[c4].has_piece = true;
    data[c4].piece = pce_get_white_pawn();
    data[e4].has_piece = true;
    data[e4].piece = pce_get_white_pawn();

    // RANK 5
    data[a5].has_piece = true;
    data[a5].piece = pce_get_black_knight();
    data[b5].has_piece = true;
    data[b5].piece = pce_get_black_pawn();
    data[d5].has_piece = true;
    data[d5].piece = pce_get_black_pawn();
    data[e5].has_piece = true;
    data[e5].piece = pce_get_black_pawn();
    data[g5].has_piece = true;
    data[g5].piece = pce_get_black_pawn();

    // RANK 6
    data[f6].has_piece = true;
    data[f6].piece = pce_get_black_knight();

    // RANK 7
    data[a7].has_piece = true;
    data[a7].piece = pce_get_black_pawn();
    data[c7].has_piece = true;
    data[c7].piece = pce_get_black_pawn();
    data[d7].has_piece = true;
    data[d7].piece = pce_get_black_king();
    data[e7].has_piece = true;
    data[e7].piece = pce_get_black_queen();
    data[f7].has_piece = true;
    data[f7].piece = pce_get_black_pawn();
    data[h7].has_piece = true;
    data[h7].piece = pce_get_black_pawn();

    // RANK 8
    data[a8].has_piece = true;
    data[a8].piece = pce_get_black_rook();
    data[h8].has_piece = true;
    data[h8].piece = pce_get_black_rook();

    struct piece *pce;
    for (int s = a1; s <= h8; s++) {
        struct sq_pce d = data[s];

        bool b = fen_try_get_piece_on_sq(brd, (enum square)s, &pce);

        if (b == true) {
            assert_true(pce == d.piece);
        }
    }
}

void test_fen_side_to_move(void **state) {
    const char *RANDOM_FEN_1 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 w Qkq - 1 4\n";
    const char *RANDOM_FEN_2 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 b KQkq - 0 3\n";

    struct parsed_fen *brd = fen_parse(RANDOM_FEN_1);
    enum colour side = fen_get_side_to_move(brd);
    assert_true(side == WHITE);

    brd = fen_parse(RANDOM_FEN_2);
    side = fen_get_side_to_move(brd);
    assert_true(side == BLACK);
}

void test_fen_castle_permissions_initial_fen(void **state) {
    struct parsed_fen *fen = fen_parse(INITIAL_FEN);

    assert_true(fen_has_wk_castle_perms(fen));
    assert_true(fen_has_wq_castle_perms(fen));
    assert_true(fen_has_bk_castle_perms(fen));
    assert_true(fen_has_bq_castle_perms(fen));
}

void test_fen_castle_permissions_random_fen(void **state) {
    const char *RANDOM_FEN_1 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 w Qkq - 1 4\n";
    const char *RANDOM_FEN_2 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 b KQkq - 0 3\n";
    const char *RANDOM_FEN_3 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b Kq - 1 2\n";
    const char *RANDOM_FEN_4 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b - - 0 3\n";

    struct parsed_fen *fen = fen_parse(RANDOM_FEN_1);

    assert_false(fen_has_wk_castle_perms(fen));
    assert_true(fen_has_wq_castle_perms(fen));
    assert_true(fen_has_bk_castle_perms(fen));
    assert_true(fen_has_wq_castle_perms(fen));

    fen = fen_parse(RANDOM_FEN_2);
    assert_true(fen_has_wk_castle_perms(fen));
    assert_true(fen_has_wq_castle_perms(fen));
    assert_true(fen_has_bk_castle_perms(fen));
    assert_true(fen_has_wq_castle_perms(fen));

    fen = fen_parse(RANDOM_FEN_3);
    assert_true(fen_has_wk_castle_perms(fen));
    assert_false(fen_has_wq_castle_perms(fen));
    assert_false(fen_has_bk_castle_perms(fen));
    assert_true(fen_has_bq_castle_perms(fen));

    fen = fen_parse(RANDOM_FEN_4);
    assert_false(fen_has_wk_castle_perms(fen));
    assert_false(fen_has_wq_castle_perms(fen));
    assert_false(fen_has_bk_castle_perms(fen));
    assert_false(fen_has_bq_castle_perms(fen));
}

void test_fen_en_passant(void **state) {
    const char *RANDOM_FEN_1 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 w Qkq f6 22 4\n";
    const char *RANDOM_FEN_2 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 b KQkq c6 11 3\n";
    const char *RANDOM_FEN_3 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 b Kq b3 1 2\n";
    const char *RANDOM_FEN_4 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b - g3 0 3\n";
    const char *RANDOM_FEN_5 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b - - 0 3\n";

    struct parsed_fen *brd = fen_parse(RANDOM_FEN_1);
    enum square enp_sq = fen_get_en_pass_sq(brd);
    assert_true(enp_sq == f6);

    brd = fen_parse(RANDOM_FEN_2);
    enp_sq = fen_get_en_pass_sq(brd);
    assert_true(enp_sq == c6);

    brd = fen_parse(RANDOM_FEN_3);
    enp_sq = fen_get_en_pass_sq(brd);
    assert_true(enp_sq == b3);

    brd = fen_parse(RANDOM_FEN_4);
    enp_sq = fen_get_en_pass_sq(brd);
    assert_true(enp_sq == g3);

    brd = fen_parse(RANDOM_FEN_5);
    enp_sq = fen_get_en_pass_sq(brd);
    assert_true(enp_sq == NO_SQUARE);
}

void test_fen_half_move_count(void **state) {
    const char *RANDOM_FEN_1 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 w Qkq - 22 4\n";
    const char *RANDOM_FEN_2 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 b KQkq - 11 3\n";
    const char *RANDOM_FEN_3 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b Kq - 1 2\n";
    const char *RANDOM_FEN_4 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b - - 0 3\n";

    struct parsed_fen *brd = fen_parse(RANDOM_FEN_1);
    assert_int_equal(fen_get_half_move_cnt(brd), 22);
    brd = fen_parse(RANDOM_FEN_2);
    assert_int_equal(fen_get_half_move_cnt(brd), 11);
    brd = fen_parse(RANDOM_FEN_3);
    assert_int_equal(fen_get_half_move_cnt(brd), 1);
    brd = fen_parse(RANDOM_FEN_4);
    assert_int_equal(fen_get_half_move_cnt(brd), 0);
}

void test_fen_full_move_count(void **state) {
    const char *RANDOM_FEN_1 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 w Qkq - 22 4\n";
    const char *RANDOM_FEN_2 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/"
                               "1B1PQP1P/R4RK1 b KQkq - 11 3\n";
    const char *RANDOM_FEN_3 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b Kq - 1 2\n";
    const char *RANDOM_FEN_4 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b - - 0 10\n";

    struct parsed_fen *brd = fen_parse(RANDOM_FEN_1);
    assert_int_equal(fen_get_full_move_cnt(brd), 4);
    brd = fen_parse(RANDOM_FEN_2);
    assert_int_equal(fen_get_full_move_cnt(brd), 3);
    brd = fen_parse(RANDOM_FEN_3);
    assert_int_equal(fen_get_full_move_cnt(brd), 2);
    brd = fen_parse(RANDOM_FEN_4);
    assert_int_equal(fen_get_full_move_cnt(brd), 10);
}

#pragma GCC diagnostic pop

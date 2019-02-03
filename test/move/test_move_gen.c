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
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"

#include "test_move_gen.h"
#include "board.h"
#include "fen.h"
#include "move_gen.h"
#include "move_list.h"
#include "piece.h"
#include "position.h"
#include <cmocka.h>

static bool contains_all_4_promotion_moves(const enum square from_sq,
                                           const enum square to_sq,
                                           struct move_list *mvl,
                                           const bool is_capture);

#define CONTAINS_QUIET(mvl, from, to)                                          \
    ({                                                                         \
        struct move mv;                                                        \
        mv = move_encode_quiet(from, to);                                      \
        assert_true(mvl_contains_move(mvl, mv));                               \
    })
#define CONTAINS_CAPTURE(mvl, from, to)                                        \
    ({                                                                         \
        struct move mv;                                                        \
        mv = move_encode_capture(from, to);                                    \
        assert_true(mvl_contains_move(mvl, mv));                               \
    })

void test_move_gen_knight_white_1(void **state) {
    const char *RANDOM_FEN_1 =
        "R1n2b2/3p4/K1P2n2/1P2N2p/P2k1pN1/1P2p1Q1/Rpb1p3/1rB5 w - - 0 1\n";
    const uint16_t NUM_EXPECTED_WHITE_KNIGHT_MOVES = 11;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct board *brd = pos_get_board(pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_knight_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_WHITE_KNIGHT_MOVES);

    CONTAINS_QUIET(mvl, g4, f2);
    CONTAINS_QUIET(mvl, g4, h2);
    CONTAINS_QUIET(mvl, g4, h6);
    CONTAINS_QUIET(mvl, e5, c4);
    CONTAINS_QUIET(mvl, e5, d3);
    CONTAINS_QUIET(mvl, e5, f3);
    CONTAINS_QUIET(mvl, e5, g6);
    CONTAINS_QUIET(mvl, e5, f7);

    CONTAINS_CAPTURE(mvl, g4, e3);
    CONTAINS_CAPTURE(mvl, g4, f6);
    CONTAINS_CAPTURE(mvl, e5, d7);
}

void test_move_gen_knight_white_2(void **state) {
    const char *RANDOM_FEN_1 = "1N3b1k/1bRRPrp1/QP1P2B1/1P1P2Bn/pK1p3p/2P1Np1p/"
                               "1pPPpq1n/4r3 w - - 0 1\n";
    const uint16_t NUM_EXPECTED_WHITE_KNIGHT_MOVES = 7;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct board *brd = pos_get_board(pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_knight_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_WHITE_KNIGHT_MOVES);

    CONTAINS_QUIET(mvl, b8, c6);
    CONTAINS_QUIET(mvl, e3, c4);
    CONTAINS_QUIET(mvl, e3, d1);
    CONTAINS_QUIET(mvl, e3, f1);
    CONTAINS_QUIET(mvl, e3, g2);
    CONTAINS_QUIET(mvl, e3, g4);
    CONTAINS_QUIET(mvl, e3, f5);
}

void test_move_gen_knight_white_3(void **state) {
    const char *RANDOM_FEN_1 =
        "K3b1nk/3p3p/2pPNqP1/1pQPp1PB/1p4Pp/pP6/P1PRrN1B/3nb1Rr w - - 0 1\n";
    const uint16_t NUM_EXPECTED_WHITE_KNIGHT_MOVES = 11;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct board *brd = pos_get_board(pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_knight_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_WHITE_KNIGHT_MOVES);

    CONTAINS_QUIET(mvl, f2, e4);
    CONTAINS_QUIET(mvl, f2, d3);
    CONTAINS_QUIET(mvl, f2, h3);
    CONTAINS_QUIET(mvl, e6, c7);
    CONTAINS_QUIET(mvl, e6, d4);
    CONTAINS_QUIET(mvl, e6, d8);
    CONTAINS_QUIET(mvl, e6, f4);
    CONTAINS_QUIET(mvl, e6, g7);
    CONTAINS_QUIET(mvl, e6, f8);

    CONTAINS_CAPTURE(mvl, f2, d1);
    CONTAINS_CAPTURE(mvl, f2, h1);
}

void test_move_gen_knight_black_1(void **state) {
    const char *RANDOM_FEN_1 =
        "R1n2b2/3p4/K1P2n2/1P2N2p/P2k1pN1/1P2p1Q1/Rpb1p3/1rB5 b - - 0 1\n";
    const uint16_t NUM_EXPECTED_BLACK_KNIGHT_MOVES = 10;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct board *brd = pos_get_board(pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_knight_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_BLACK_KNIGHT_MOVES);

    CONTAINS_QUIET(mvl, c8, a7);
    CONTAINS_QUIET(mvl, c8, b6);
    CONTAINS_QUIET(mvl, c8, d6);
    CONTAINS_QUIET(mvl, c8, e7);
    CONTAINS_QUIET(mvl, f6, e8);
    CONTAINS_QUIET(mvl, f6, d5);
    CONTAINS_QUIET(mvl, f6, e4);
    CONTAINS_QUIET(mvl, f6, h7);
    CONTAINS_QUIET(mvl, f6, g8);

    CONTAINS_CAPTURE(mvl, f6, g4);
}

void test_move_gen_knight_black_2(void **state) {
    const char *RANDOM_FEN_1 =
        "1r5r/1P1PPP2/ppB1KN2/1p5q/2nR1PPP/p3Pnpp/kpNRQ1pb/4B2b b - - 0 1\n";
    const uint16_t NUM_EXPECTED_BLACK_KNIGHT_MOVES = 12;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct board *brd = pos_get_board(pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_knight_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_BLACK_KNIGHT_MOVES);

    CONTAINS_QUIET(mvl, c4, a5);
    CONTAINS_QUIET(mvl, c4, e5);
    CONTAINS_QUIET(mvl, c4, d6);
    CONTAINS_QUIET(mvl, f3, g1);
    CONTAINS_QUIET(mvl, f3, g5);
    CONTAINS_QUIET(mvl, f3, e5);

    // check capture moves
    CONTAINS_CAPTURE(mvl, c4, e3);
    CONTAINS_CAPTURE(mvl, c4, d2);
    CONTAINS_CAPTURE(mvl, f3, e1);
    CONTAINS_CAPTURE(mvl, f3, d2);
    CONTAINS_CAPTURE(mvl, f3, d4);
    CONTAINS_CAPTURE(mvl, f3, h4);
}

void test_move_gen_knight_black_3(void **state) {
    const char *RANDOM_FEN_1 =
        "2K5/ppbQpr1q/N2PPBP1/kB1bP1N1/2P4p/1pP4p/1npnPPp1/rR2R3 b - - 0 1\n";
    const uint16_t NUM_EXPECTED_BLACK_KNIGHT_MOVES = 9;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct board *brd = pos_get_board(pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_knight_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_BLACK_KNIGHT_MOVES);

    CONTAINS_QUIET(mvl, b2, d1);
    CONTAINS_QUIET(mvl, b2, d3);
    CONTAINS_QUIET(mvl, b2, a4);
    CONTAINS_QUIET(mvl, d2, f1);
    CONTAINS_QUIET(mvl, d2, f3);
    CONTAINS_QUIET(mvl, d2, e4);

    CONTAINS_CAPTURE(mvl, b2, c4);
    CONTAINS_CAPTURE(mvl, d2, c4);
    CONTAINS_CAPTURE(mvl, d2, b1);
}

void test_move_gen_king_white_no_castling_1(void **state) {
    const char *RANDOM_FEN_1 =
        "5Q2/1p3pp1/3P1Pn1/6k1/2K5/Pr3p2/p2R1P2/7B w - - 0 1\n";
    const uint16_t NUM_EXPECTED_MOVES = 8;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_MOVES);

    CONTAINS_QUIET(mvl, c4, c3);
    CONTAINS_QUIET(mvl, c4, d3);
    CONTAINS_QUIET(mvl, c4, d4);
    CONTAINS_QUIET(mvl, c4, d5);
    CONTAINS_QUIET(mvl, c4, c5);
    CONTAINS_QUIET(mvl, c4, b5);
    CONTAINS_QUIET(mvl, c4, b4);

    CONTAINS_CAPTURE(mvl, c4, b3);
}

void test_move_gen_king_white_no_castling_2(void **state) {
    const char *RANDOM_FEN_1 =
        "8/3Q2pN/1b5P/3Pp2K/2n3p1/P2RP3/Pp6/5k2 w - - 0 1\n";
    const uint16_t NUM_EXPECTED_MOVES = 4;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_MOVES);

    CONTAINS_QUIET(mvl, h5, g6);
    CONTAINS_QUIET(mvl, h5, g5);
    CONTAINS_QUIET(mvl, h5, h4);

    CONTAINS_CAPTURE(mvl, h5, g4);
}

void test_move_gen_king_white_no_castling_3(void **state) {
    const char *RANDOM_FEN_1 =
        "n7/4p2p/PP1p1pp1/1n4k1/6q1/6N1/K3p3/2N3r1 w - - 0 1\n";
    const uint16_t NUM_EXPECTED_MOVES = 5;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_MOVES);

    CONTAINS_QUIET(mvl, a2, a3);
    CONTAINS_QUIET(mvl, a2, b3);
    CONTAINS_QUIET(mvl, a2, b2);
    CONTAINS_QUIET(mvl, a2, b1);
    CONTAINS_QUIET(mvl, a2, a1);
}

void test_move_white_castling_WK_WQ(void **state) {
    const char *RANDOM_FEN_1 =
        "r1bqkb1r/ppp2ppp/2n2n2/3pp3/2PP4/2NQPN2/PP1BBPPP/R3K2R w KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, WHITE, mvl);

    struct move wk_cast = move_encode_castle_kingside();
    assert_true(mvl_contains_move(mvl, wk_cast));

    struct move wq_cast = move_encode_castle_queenside();
    assert_true(mvl_contains_move(mvl, wq_cast));
}

void test_move_white_castling_WK_only(void **state) {
    const char *RANDOM_FEN_1 =
        "r1bqkb1r/ppp2ppp/2n2n2/3pp3/2PP4/2NQPN2/PP2BPPP/R1B1K2R w K - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, WHITE, mvl);

    struct move wk_cast = move_encode_castle_kingside();
    assert_true(mvl_contains_move(mvl, wk_cast));

    struct move wq_cast = move_encode_castle_queenside();
    assert_false(mvl_contains_move(mvl, wq_cast));
}

void test_move_white_castling_WQ_only(void **state) {
    const char *RANDOM_FEN_1 =
        "r1bqkb1r/ppp2ppp/2n2n2/3pp3/2PP4/2NQP3/PP1B1PPP/R3K1NR w Q - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, WHITE, mvl);

    struct move wk_cast = move_encode_castle_kingside();
    assert_false(mvl_contains_move(mvl, wk_cast));

    struct move wq_cast = move_encode_castle_queenside();
    assert_true(mvl_contains_move(mvl, wq_cast));
}

void test_move_gen_king_black_no_castling_1(void **state) {
    const char *RANDOM_FEN_1 =
        "5Q2/1p3pp1/3P1Pn1/6k1/2K5/Pr3p2/p2R1P2/7B w - - 0 1\n";
    const uint16_t NUM_EXPECTED_MOVES = 7;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_MOVES);

    CONTAINS_QUIET(mvl, g5, h6);
    CONTAINS_QUIET(mvl, g5, h5);
    CONTAINS_QUIET(mvl, g5, h4);
    CONTAINS_QUIET(mvl, g5, g4);
    CONTAINS_QUIET(mvl, g5, f4);
    CONTAINS_QUIET(mvl, g5, f5);

    CONTAINS_CAPTURE(mvl, g5, f6);
}

void test_move_gen_king_black_no_castling_2(void **state) {
    const char *RANDOM_FEN_1 =
        "8/3Q2pN/1b5P/3Pp2K/2n3p1/P2RP3/Pp6/5k2 w - - 0 1\n";
    const uint16_t NUM_EXPECTED_MOVES = 5;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_MOVES);

    CONTAINS_QUIET(mvl, f1, g1);
    CONTAINS_QUIET(mvl, f1, g2);
    CONTAINS_QUIET(mvl, f1, f2);
    CONTAINS_QUIET(mvl, f1, e2);
    CONTAINS_QUIET(mvl, f1, e1);
}

void test_move_gen_king_black_no_castling_3(void **state) {
    const char *RANDOM_FEN_1 =
        "n7/4p2p/PP1p1pp1/1n4k1/6q1/6N1/K3p3/2N3r1 w - - 0 1\n";
    const uint16_t NUM_EXPECTED_MOVES = 5;

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == NUM_EXPECTED_MOVES);

    CONTAINS_QUIET(mvl, g5, h6);
    CONTAINS_QUIET(mvl, g5, h5);
    CONTAINS_QUIET(mvl, g5, h4);
    CONTAINS_QUIET(mvl, g5, f4);
    CONTAINS_QUIET(mvl, g5, f5);
}

void test_move_black_castling_BK_BQ(void **state) {
    const char *RANDOM_FEN_1 =
        "r3k2r/pppqbppp/2n1bn2/3pp3/2PP4/2NQP3/PP1B1PPP/R3K1NR b Qkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, BLACK, mvl);

    struct move bk_cast = move_encode_castle_kingside();
    assert_true(mvl_contains_move(mvl, bk_cast));

    struct move bq_cast = move_encode_castle_queenside();
    assert_true(mvl_contains_move(mvl, bq_cast));
}

void test_move_black_castling_BK_only(void **state) {
    const char *RANDOM_FEN_1 =
        "rn2k2r/pppqbppp/4bn2/3pp3/2PP4/2NQP3/PP1B1PPP/R3K1NR w Qk - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, BLACK, mvl);

    struct move bk_cast = move_encode_castle_kingside();
    assert_true(mvl_contains_move(mvl, bk_cast));

    struct move bq_cast = move_encode_castle_queenside();
    assert_false(mvl_contains_move(mvl, bq_cast));
}

void test_move_black_castling_BQ_only(void **state) {
    const char *RANDOM_FEN_1 =
        "r3k1nr/pppqbppp/2n1b3/3pp3/2PP4/2NQP3/PP1B1PPP/R3K1NR w Qq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_king_moves(pos, BLACK, mvl);

    struct move bk_cast = move_encode_castle_kingside();
    assert_false(mvl_contains_move(mvl, bk_cast));

    struct move bq_cast = move_encode_castle_queenside();
    assert_true(mvl_contains_move(mvl, bq_cast));
}

void test_move_white_pawns_promotion_1(void **state) {
    const char *RANDOM_FEN_1 = "2r1N3/pPp1QpnP/Np1B2p1/1pP1R1PP/r2p2b1/3P2RB/"
                               "P1Kp1k1P/bn5q w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    assert_true(contains_all_4_promotion_moves(b7, b8, mvl, false));
    assert_true(contains_all_4_promotion_moves(b7, c8, mvl, true));
    assert_true(contains_all_4_promotion_moves(h7, h8, mvl, false));
}

static bool contains_all_4_promotion_moves(const enum square from_sq,
                                           const enum square to_sq,
                                           struct move_list *mvl,
                                           const bool is_capture) {
    struct move mv = move_encode_promoted(from_sq, to_sq, WKNIGHT, is_capture);
    if (mvl_contains_move(mvl, mv) == false) {
        return false;
    }
    mv = move_encode_promoted(from_sq, to_sq, WBISHOP, is_capture);
    if (mvl_contains_move(mvl, mv) == false) {
        return false;
    }
    mv = move_encode_promoted(from_sq, to_sq, WROOK, is_capture);
    if (mvl_contains_move(mvl, mv) == false) {
        return false;
    }
    mv = move_encode_promoted(from_sq, to_sq, WQUEEN, is_capture);
    if (mvl_contains_move(mvl, mv) == false) {
        return false;
    }
    return true;
}

void test_move_white_pawns_promotion_2(void **state) {
    const char *RANDOM_FEN_1 = "bbr1n1B1/1P1PNPpP/1pN1Pkn1/P1r3p1/pBR2P1Q/"
                               "pq1p3p/1R1P3p/7K w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    assert_true(contains_all_4_promotion_moves(b7, a8, mvl, true));
    assert_true(contains_all_4_promotion_moves(b7, c8, mvl, true));
    assert_true(contains_all_4_promotion_moves(d7, c8, mvl, true));
    assert_true(contains_all_4_promotion_moves(d7, d8, mvl, false));
    assert_true(contains_all_4_promotion_moves(d7, e8, mvl, true));
    assert_true(contains_all_4_promotion_moves(f7, e8, mvl, true));
    assert_true(contains_all_4_promotion_moves(f7, f8, mvl, false));
    assert_true(contains_all_4_promotion_moves(h7, h8, mvl, false));
}

void test_move_white_pawns_1(void **state) {
    const char *RANDOM_FEN_1 =
        "1qN5/pR1B4/2Pp1Pbb/Bp1Pr1pP/k1P3pp/NrP1P1nP/K1p2n1p/2R4Q w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    assert_true(mvl_get_move_count(mvl) == 7);

    CONTAINS_QUIET(mvl, c6, c7);
    CONTAINS_QUIET(mvl, c4, c5);
    CONTAINS_QUIET(mvl, e3, e4);
    CONTAINS_QUIET(mvl, f6, f7);

    CONTAINS_CAPTURE(mvl, c4, b5);
    CONTAINS_CAPTURE(mvl, h5, g6);
    CONTAINS_CAPTURE(mvl, h3, g4);
}

void test_move_white_pawns_2(void **state) {
    const char *RANDOM_FEN_1 =
        "1Nr1n3/P2pkp2/pp1bb1pK/Q1pq3P/NP4pp/P3PR1P/B3P1P1/2B2nRr w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    assert_true(mvl_get_move_count(mvl) == 10);
    assert_true(contains_all_4_promotion_moves(a7, a8, mvl, false));

    CONTAINS_QUIET(mvl, b4, b5);
    CONTAINS_QUIET(mvl, e3, e4);
    CONTAINS_QUIET(mvl, g2, g3);

    CONTAINS_CAPTURE(mvl, b4, c5);
    CONTAINS_CAPTURE(mvl, h3, g4);
    CONTAINS_CAPTURE(mvl, h5, g6);
}

void test_move_white_pawns_first_move_double_1(void **state) {
    const char *RANDOM_FEN_1 =
        "3N1Kb1/3Q1N1p/1pPnP1P1/p1Rr1ppp/4B1rB/2p5/P1PPqPPR/3n1k2 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_pawn_double_first(a2, a4);
    assert_true(mvl_contains_move(mvl, mv));

    mv = move_encode_pawn_double_first(d2, d4);
    assert_true(mvl_contains_move(mvl, mv));

    mv = move_encode_pawn_double_first(f2, f4);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_white_pawns_first_move_double_2(void **state) {
    const char *RANDOM_FEN_1 = "3N1Kb1/3Q1N1p/1p1nP1P1/p1Rr1ppp/2p1B1rB/2P3P1/"
                               "PP1PqP1R/3n1k2 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_pawn_double_first(a2, a4);
    assert_true(mvl_contains_move(mvl, mv));

    mv = move_encode_pawn_double_first(b2, b4);
    assert_true(mvl_contains_move(mvl, mv));

    mv = move_encode_pawn_double_first(d2, d4);
    assert_true(mvl_contains_move(mvl, mv));

    mv = move_encode_pawn_double_first(f2, f4);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_white_pawns_en_passant_1(void **state) {
    const char *RANDOM_FEN_1 = "3N1Kb1/3Q1N2/1p1nP3/p1Rr1pPp/2p1B1rB/2P3P1/"
                               "PP1PqP1R/3n1k2 w - h6 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_enpassant(g5, h6);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_white_pawns_en_passant_2(void **state) {
    const char *RANDOM_FEN_1 =
        "3N1Kb1/3Q1N2/3n4/pPpr1pPp/2p1B1rB/2P3P1/PP1PqP1R/3n1k2 w - a6 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_enpassant(b5, a6);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_white_pawns_en_passant_3(void **state) {
    const char *RANDOM_FEN_1 =
        "3N1Kb1/3Q1N2/3n4/pPpr1pPp/2p1B1rB/2P3P1/PP1PqP1R/3n1k2 w - c6 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_white_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_enpassant(b5, c6);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_black_pawns_promotion_1(void **state) {
    const char *RANDOM_FEN_1 =
        "2r1N3/pPp1QpnP/Np1B4/1pP1R1PP/r3qb2/2nP4/PpKp1kpP/1b3R1B b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    assert_true(contains_all_4_promotion_moves(d2, d1, mvl, false));
    assert_true(contains_all_4_promotion_moves(g2, f1, mvl, true));
    assert_true(contains_all_4_promotion_moves(g2, g1, mvl, false));
    assert_true(contains_all_4_promotion_moves(g2, h1, mvl, true));
}

void test_move_black_pawns_promotion_2(void **state) {
    const char *RANDOM_FEN_1 = "bbr1n1B1/1P1P1PpP/1pN1Pk2/P1r3p1/pB3P1Q/1q6/"
                               "p2pK1pp/PRR1n1N1 b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    assert_true(contains_all_4_promotion_moves(a2, b1, mvl, true));
    assert_true(contains_all_4_promotion_moves(d2, c1, mvl, true));
    assert_true(contains_all_4_promotion_moves(d2, d1, mvl, false));
    assert_true(contains_all_4_promotion_moves(h2, g1, mvl, true));
    assert_true(contains_all_4_promotion_moves(h2, h1, mvl, false));
}

void test_move_black_pawns_1(void **state) {
    const char *RANDOM_FEN_1 =
        "1qN5/pR1B4/2Pp1Pbb/1pP1r1pP/k2P1p1p/Nr2P1BP/K1p2n1p/2R4Q b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    assert_true(mvl_get_move_count(mvl) == 10);

    struct move double_mv = move_encode_pawn_double_first(a7, a5);
    assert_true(mvl_contains_move(mvl, double_mv));

    CONTAINS_QUIET(mvl, a7, a6);
    CONTAINS_QUIET(mvl, b5, b4);
    CONTAINS_QUIET(mvl, f4, f3);
    CONTAINS_QUIET(mvl, d6, d5);
    CONTAINS_QUIET(mvl, g5, g4);

    CONTAINS_CAPTURE(mvl, d6, c5);
    CONTAINS_CAPTURE(mvl, f4, e3);
    CONTAINS_CAPTURE(mvl, f4, g3);
    CONTAINS_CAPTURE(mvl, h4, g3);
}

void test_move_black_pawns_2(void **state) {
    const char *RANDOM_FEN_1 =
        "1Nr1n3/P3k3/p2bb1pK/Q1pq3P/NP1p1pp1/P3PR1P/Bp2P1Pp/2B2nRr b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    assert_true(mvl_get_move_count(mvl) == 22);

    assert_true(contains_all_4_promotion_moves(b2, b1, mvl, false));
    assert_true(contains_all_4_promotion_moves(b2, c1, mvl, true));
    assert_true(contains_all_4_promotion_moves(h2, g1, mvl, true));

    CONTAINS_QUIET(mvl, c5, c4);
    CONTAINS_QUIET(mvl, g6, g5);
    CONTAINS_QUIET(mvl, g4, g3);
    CONTAINS_QUIET(mvl, d4, d3);

    CONTAINS_CAPTURE(mvl, c5, b4);
    CONTAINS_CAPTURE(mvl, d4, e3);
    CONTAINS_CAPTURE(mvl, f4, e3);
    CONTAINS_CAPTURE(mvl, g6, h5);
    CONTAINS_CAPTURE(mvl, g4, f3);
}

void test_move_black_pawns_first_move_double_1(void **state) {
    const char *RANDOM_FEN_1 =
        "3Nk1b1/pp3p1p/3n4/2RrNpp1/2P1B1rB/QPp2K1P/P1PPqPPR/3n4 b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_pawn_double_first(a7, a5);
    assert_true(mvl_contains_move(mvl, mv));

    mv = move_encode_pawn_double_first(b7, b5);
    assert_true(mvl_contains_move(mvl, mv));

    mv = move_encode_pawn_double_first(h7, h5);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_black_pawns_first_move_double_2(void **state) {
    const char *RANDOM_FEN_1 =
        "4k1b1/pp3p1p/1N1n2p1/2RrN2r/2P1Bp2/QPp2K1P/P1PPqPPR/3n4 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_pawn_double_first(a7, a5);
    assert_true(mvl_contains_move(mvl, mv));

    mv = move_encode_pawn_double_first(f7, f5);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_black_pawns_en_passant_1(void **state) {
    const char *RANDOM_FEN_1 =
        "3N1Kb1/3Q1N2/3nP3/p1Rr1pPp/Ppp1B1rB/2P3P1/1P1PqP1R/3n1k2 b - a3 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_enpassant(b4, a3);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_black_pawns_en_passant_2(void **state) {
    const char *RANDOM_FEN_1 = "3N1Kb1/3Q1N2/3nP3/p1Rr2Pp/1ppBBPp1/P1P1r1P1/"
                               "1P1Pq2R/3n1k2 b - f3 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_enpassant(g4, f3);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_black_pawns_en_passant_3(void **state) {
    const char *RANDOM_FEN_1 =
        "3N1Kb1/1q3N2/3n4/pPpr2Pp/2p1PprB/2P3PQ/PPBP3R/3n1k2 w - e3 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_black_pawn_moves(pos, brd, mvl);

    struct move mv = move_encode_enpassant(f4, e3);
    assert_true(mvl_contains_move(mvl, mv));
}

void test_move_white_bishop_1(void **state) {
    const char *RANDOM_FEN_1 =
        "3R2K1/1PknP3/p6P/Prn1Pp1p/NN3P2/r2B2Pp/p2pb3/6B1 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_bishop_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == 12);

    CONTAINS_QUIET(mvl, g1, f2);
    CONTAINS_QUIET(mvl, d3, c2);
    CONTAINS_QUIET(mvl, d3, b1);
    CONTAINS_QUIET(mvl, d3, e4);
    CONTAINS_QUIET(mvl, g1, e3);
    CONTAINS_QUIET(mvl, g1, h2);
    CONTAINS_QUIET(mvl, d3, c4);
    CONTAINS_QUIET(mvl, g1, d4);

    CONTAINS_CAPTURE(mvl, g1, c5);
    CONTAINS_CAPTURE(mvl, d3, e2);
    CONTAINS_CAPTURE(mvl, d3, b5);
    CONTAINS_CAPTURE(mvl, d3, f5);
}

void test_move_white_bishop_2(void **state) {
    const char *RANDOM_FEN_1 =
        "3n1N1n/1B1R1prK/Q2P4/P1P2r2/PPp2k1p/2B3N1/ppP4p/4b2b w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_bishop_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == 15);

    CONTAINS_QUIET(mvl, b7, a8);
    CONTAINS_QUIET(mvl, b7, c8);
    CONTAINS_QUIET(mvl, b7, c6);
    CONTAINS_QUIET(mvl, b7, d5);
    CONTAINS_QUIET(mvl, b7, e4);
    CONTAINS_QUIET(mvl, b7, f3);
    CONTAINS_QUIET(mvl, b7, g2);
    CONTAINS_QUIET(mvl, c3, d4);
    CONTAINS_QUIET(mvl, c3, e5);
    CONTAINS_QUIET(mvl, c3, f6);
    CONTAINS_QUIET(mvl, c3, d2);

    CONTAINS_CAPTURE(mvl, b7, h1);
    CONTAINS_CAPTURE(mvl, c3, b2);
    CONTAINS_CAPTURE(mvl, c3, e1);
    CONTAINS_CAPTURE(mvl, c3, g7);
}

void test_move_white_bishop_3(void **state) {
    const char *RANDOM_FEN_1 =
        "3n1N1n/3R1pr1/Q2P2bK/P1P2r2/PPp1Bk1p/6N1/ppP4p/B3b3 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_bishop_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == 10);

    CONTAINS_QUIET(mvl, e4, d3);
    CONTAINS_QUIET(mvl, e4, d5);
    CONTAINS_QUIET(mvl, e4, c6);
    CONTAINS_QUIET(mvl, e4, b7);
    CONTAINS_QUIET(mvl, e4, a8);
    CONTAINS_QUIET(mvl, e4, f3);
    CONTAINS_QUIET(mvl, e4, g2);
    CONTAINS_QUIET(mvl, e4, h1);

    CONTAINS_CAPTURE(mvl, a1, b2);
    CONTAINS_CAPTURE(mvl, e4, f5);
}

void test_move_black_bishop_1(void **state) {
    const char *RANDOM_FEN_1 = "4q1Q1/bp2P1P1/PRPrnN1P/P3k1p1/1r1pp2B/1Rp1p2P/"
                               "pnPN4/1B2Kb2 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_bishop_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == 9);

    CONTAINS_QUIET(mvl, a7, b8);
    CONTAINS_QUIET(mvl, f1, e2);
    CONTAINS_QUIET(mvl, f1, d3);
    CONTAINS_QUIET(mvl, f1, c4);
    CONTAINS_QUIET(mvl, f1, b5);
    CONTAINS_QUIET(mvl, f1, g2);

    CONTAINS_CAPTURE(mvl, a7, b6);
    CONTAINS_CAPTURE(mvl, f1, a6);
    CONTAINS_CAPTURE(mvl, f1, h3);
}

void test_move_black_bishop_2(void **state) {
    const char *RANDOM_FEN_1 = "2bK2n1/Pp1p1PP1/R1P1r1PP/1R1P1QP1/1p2pB2/"
                               "bq3pp1/1r2pnp1/NB2k2N b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_bishop_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == 0);
}

void test_move_black_bishop_3(void **state) {
    const char *RANDOM_FEN_1 =
        "2N1r3/2k2P2/b7/8/r2P1p2/2B4P/1K4b1/8 b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_bishop_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == 16);

    CONTAINS_QUIET(mvl, a6, b7);
    CONTAINS_QUIET(mvl, a6, b5);
    CONTAINS_QUIET(mvl, a6, c4);
    CONTAINS_QUIET(mvl, a6, d3);
    CONTAINS_QUIET(mvl, a6, e2);
    CONTAINS_QUIET(mvl, a6, f1);
    CONTAINS_QUIET(mvl, g2, f1);
    CONTAINS_QUIET(mvl, g2, h1);
    CONTAINS_QUIET(mvl, g2, f3);
    CONTAINS_QUIET(mvl, g2, e4);
    CONTAINS_QUIET(mvl, g2, d5);
    CONTAINS_QUIET(mvl, g2, c6);
    CONTAINS_QUIET(mvl, g2, b7);
    CONTAINS_QUIET(mvl, g2, a8);

    CONTAINS_CAPTURE(mvl, a6, c8);
    CONTAINS_CAPTURE(mvl, g2, h3);
}

void test_move_white_rook_1(void **state) {
    const char *RANDOM_FEN_1 =
        "3kb2B/RP1P4/P4ppN/P1P1Rq2/2PKn1pp/1rQ3p1/2pp1NpP/rnb5 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_rook_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == 7);

    CONTAINS_QUIET(mvl, a7, a8);
    CONTAINS_QUIET(mvl, e5, d5);
    CONTAINS_QUIET(mvl, e5, e6);
    CONTAINS_QUIET(mvl, e5, e7);

    CONTAINS_CAPTURE(mvl, e5, f5);
    CONTAINS_CAPTURE(mvl, e5, e8);
    CONTAINS_CAPTURE(mvl, e5, e4);
}

void test_move_white_rook_2(void **state) {
    const char *RANDOM_FEN_1 = "R2Rb3/1qP2p1P/1Nr2k1p/2bp2pK/2p2B2/1r1ppBP1/"
                               "1PPN1PPp/1n2Q3 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_rook_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == 15);

    CONTAINS_QUIET(mvl, a8, b8);
    CONTAINS_QUIET(mvl, a8, c8);
    CONTAINS_QUIET(mvl, a8, a7);
    CONTAINS_QUIET(mvl, a8, a6);
    CONTAINS_QUIET(mvl, a8, a5);
    CONTAINS_QUIET(mvl, a8, a4);
    CONTAINS_QUIET(mvl, a8, a3);
    CONTAINS_QUIET(mvl, a8, a2);
    CONTAINS_QUIET(mvl, a8, a1);
    CONTAINS_QUIET(mvl, d8, c8);
    CONTAINS_QUIET(mvl, d8, b8);
    CONTAINS_QUIET(mvl, d8, d7);
    CONTAINS_QUIET(mvl, d8, d6);

    CONTAINS_CAPTURE(mvl, d8, e8);
    CONTAINS_CAPTURE(mvl, d8, d5);
}

void test_move_white_rook_3(void **state) {
    const char *RANDOM_FEN_1 =
        "b1NRNk2/1pPp3K/p6P/Qp3rB1/nRq2p2/1r4pP/1PpPP3/b2n4 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_rook_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == 5);

    CONTAINS_CAPTURE(mvl, b4, b5);
    CONTAINS_CAPTURE(mvl, b4, a4);
    CONTAINS_CAPTURE(mvl, b4, b3);
    CONTAINS_CAPTURE(mvl, b4, c4);
    CONTAINS_CAPTURE(mvl, d8, d7);
}

void test_move_black_rook_1(void **state) {
    const char *RANDOM_FEN_1 =
        "3kb2B/RP1P4/P4ppN/P1P1Rq2/2PKn1pp/1rQ3p1/2pp1NpP/rnb5 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_rook_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == 11);

    CONTAINS_QUIET(mvl, a1, a2);
    CONTAINS_QUIET(mvl, a1, a3);
    CONTAINS_QUIET(mvl, a1, a4);
    CONTAINS_QUIET(mvl, b3, a3);
    CONTAINS_QUIET(mvl, b3, b2);
    CONTAINS_QUIET(mvl, b3, b4);
    CONTAINS_QUIET(mvl, b3, b5);
    CONTAINS_QUIET(mvl, b3, b6);

    CONTAINS_CAPTURE(mvl, a1, a5);
    CONTAINS_CAPTURE(mvl, b3, c3);
    CONTAINS_CAPTURE(mvl, b3, b7);
}

void test_move_black_rook_2(void **state) {
    // BROOK pieces on board, but  blocked from moving
    const char *RANDOM_FEN_1 =
        "3kb2B/RP1P4/P5pN/P1P1Rq2/2PK2pp/1pQ3p1/nrpp1NpP/rnb5 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_rook_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == 0);
}

void test_move_black_rook_3(void **state) {
    // No BROOK pieces on board
    const char *RANDOM_FEN_1 =
        "2q2R1n/p1P1p3/2p2n1P/PP1pP2P/1Bk1NP2/PNp1QpKb/1p4R1/2b5 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_rook_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == 0);
}

void test_move_white_queen_1(void **state) {
    // No BROOK pieces on board
    const char *RANDOM_FEN_1 =
        "q1k3rr/2p1pBB1/1K2Pp1n/Pb3Ppp/1nP1PQ2/2Pp4/5P1P/1N1N1R2 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_queen_moves(brd, WHITE, mvl);

    assert_true(mvl_get_move_count(mvl) == 11);

    CONTAINS_QUIET(mvl, f4, f3);
    CONTAINS_QUIET(mvl, f4, e3);
    CONTAINS_QUIET(mvl, f4, d2);
    CONTAINS_QUIET(mvl, f4, c1);
    CONTAINS_QUIET(mvl, f4, g3);
    CONTAINS_QUIET(mvl, f4, g4);
    CONTAINS_QUIET(mvl, f4, h4);
    CONTAINS_QUIET(mvl, f4, e5);
    CONTAINS_QUIET(mvl, f4, d6);

    CONTAINS_CAPTURE(mvl, f4, g5);
    CONTAINS_CAPTURE(mvl, f4, c7);
}

void test_move_black_queen_1(void **state) {
    // No BROOK pieces on board
    const char *RANDOM_FEN_1 =
        "q1k3rr/2p1pBB1/1K2Pp1n/Pb3Ppp/1nP1PQ2/2Pp4/5P1P/1N1N1R2 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    struct board *brd = pos_get_board(pos);
    mv_gen_queen_moves(brd, BLACK, mvl);

    assert_true(mvl_get_move_count(mvl) == 8);

    CONTAINS_QUIET(mvl, a8, b8);
    CONTAINS_QUIET(mvl, a8, a7);
    CONTAINS_QUIET(mvl, a8, a6);
    CONTAINS_QUIET(mvl, a8, b7);
    CONTAINS_QUIET(mvl, a8, c6);
    CONTAINS_QUIET(mvl, a8, d5);

    CONTAINS_CAPTURE(mvl, a8, a5);
    CONTAINS_CAPTURE(mvl, a8, e4);
}

void test_move_all_moves_4_knights_opening_white_to_move(void **state) {
    const char *RANDOM_FEN_1 = "r1bqkb1r/pppp1ppp/2n2n2/4p3/4P3/2N2N2/PPPP1PPP/"
                               "R1BQKB1R w KQkq - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);
    struct move_list *mvl = mvl_allocate();

    mv_gen_all_moves(pos, mvl);

    struct move double_mv = move_encode_pawn_double_first(a2, a4);
    assert_true(mvl_contains_move(mvl, double_mv));
    double_mv = move_encode_pawn_double_first(b2, b4);
    assert_true(mvl_contains_move(mvl, double_mv));
    double_mv = move_encode_pawn_double_first(d2, d4);
    assert_true(mvl_contains_move(mvl, double_mv));
    double_mv = move_encode_pawn_double_first(g2, g4);
    assert_true(mvl_contains_move(mvl, double_mv));
    double_mv = move_encode_pawn_double_first(h2, h4);
    assert_true(mvl_contains_move(mvl, double_mv));

    CONTAINS_QUIET(mvl, a2, a3);
    CONTAINS_QUIET(mvl, b2, b3);
    CONTAINS_QUIET(mvl, d2, d3);
    CONTAINS_QUIET(mvl, g2, g3);
    CONTAINS_QUIET(mvl, h2, h3);
    CONTAINS_QUIET(mvl, f1, e2);
    CONTAINS_QUIET(mvl, f1, d3);
    CONTAINS_QUIET(mvl, f1, c4);
    CONTAINS_QUIET(mvl, f1, b5);
    CONTAINS_QUIET(mvl, f1, a6);

    CONTAINS_QUIET(mvl, c3, a4);
    CONTAINS_QUIET(mvl, c3, b5);
    CONTAINS_QUIET(mvl, c3, d5);
    CONTAINS_QUIET(mvl, c3, e2);
    CONTAINS_QUIET(mvl, c3, b1);

    CONTAINS_QUIET(mvl, f3, d4);
    CONTAINS_QUIET(mvl, f3, g5);
    CONTAINS_QUIET(mvl, f3, h4);
    CONTAINS_QUIET(mvl, f3, g1);

    CONTAINS_QUIET(mvl, e1, e2);
    CONTAINS_QUIET(mvl, d1, e2);

    CONTAINS_QUIET(mvl, a1, b1);
    CONTAINS_QUIET(mvl, h1, g1);

    CONTAINS_CAPTURE(mvl, f3, e5);
}

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

#include "test_position.h"
#include "board.h"
#include "hashkeys.h"
#include "position.h"
#include <cmocka.h>

struct mv_from_to {
    enum square from_sq;
    enum square to_sq;
};

struct decode_move {
    struct move mv;
    enum piece decoded_piece;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"

void test_position_get_set_castle_permissions(void **state) {
    struct cast_perm_container cp = {.val = 0};
    cast_perm_set_permission(CASTLE_PERM_WK, &cp, true);
    struct position *pos = pos_create();

    pos_set_cast_perm(pos, cp);
    struct cast_perm_container retrieved_cp = pos_get_cast_perm(pos);
    assert_true(cast_compare_perms(cp, retrieved_cp));

    cast_perm_set_permission(CASTLE_PERM_WQ, &cp, true);
    pos_set_cast_perm(pos, cp);
    retrieved_cp = pos_get_cast_perm(pos);
    assert_true(cast_compare_perms(cp, retrieved_cp));

    cast_perm_set_permission(CASTLE_PERM_WK, &cp, true);
    pos_set_cast_perm(pos, cp);
    retrieved_cp = pos_get_cast_perm(pos);
    assert_true(cast_compare_perms(cp, retrieved_cp));

    cast_perm_set_permission(CASTLE_PERM_BQ, &cp, true);
    pos_set_cast_perm(pos, cp);
    retrieved_cp = pos_get_cast_perm(pos);
    assert_true(cast_compare_perms(cp, retrieved_cp));
}

void test_position_compare(void **state) {
    const char *FEN = "6Br/R3B3/5NPn/PNpn1k1r/3P4/q2pQ3/bR6/4bK2 w - - 0 1\n";

    struct position *pos1 = pos_create();
    pos_initialise(FEN, pos1);
    struct board *brd1 = pos_get_board(pos1);

    struct position *pos2 = pos_create();
    pos_initialise(FEN, pos2);

    assert_true(pos_compare(pos1, pos2));

    brd_add_piece(brd1, WHITE_PAWN, a1);
    assert_false(pos_compare(pos1, pos2));

    brd_remove_piece(brd1, WHITE_PAWN, a1);
    assert_true(pos_compare(pos1, pos2));
}

void test_position_make_move_white_double_first_move(void **state) {
    struct mv_from_to moves[8] = {{.from_sq = a2, .to_sq = a4}, {.from_sq = b2, .to_sq = b4},
                                  {.from_sq = c2, .to_sq = c4}, {.from_sq = d2, .to_sq = d4},
                                  {.from_sq = e2, .to_sq = e4}, {.from_sq = f2, .to_sq = f4},
                                  {.from_sq = g2, .to_sq = g4}, {.from_sq = h2, .to_sq = h4}};

    struct move quiet_move = move_encode_quiet(a7, a6);

    for (int i = 0; i < 8; i++) {

        const enum square from_sq = moves[i].from_sq;
        const enum square to_sq = moves[i].to_sq;

        struct position *pos = pos_create();
        pos_initialise(INITIAL_FEN, pos);

        struct move mv = move_encode_pawn_double_first(from_sq, to_sq);

        // baseline, not set
        enum square enp_sq = pos_get_en_pass_sq(pos);
        assert_true(enp_sq == NO_SQUARE);

        // make move and check en passant square
        pos_make_move(pos, mv);

        enp_sq = pos_get_en_pass_sq(pos);
        assert_true(enp_sq != NO_SQUARE);

        enum square expected_enp_sq = sq_get_square_plus_1_rank(moves[i].from_sq);
        assert_true(expected_enp_sq == enp_sq);

        // check the pawn piece has moved
        bool is_from_sq_occupied = brd_is_sq_occupied(pos_get_board(pos), from_sq);
        assert_false(is_from_sq_occupied);
        bool is_to_sq_occupied = brd_is_sq_occupied(pos_get_board(pos), to_sq);
        assert_true(is_to_sq_occupied);

        // check the side has swapped
        enum colour side_to_move = pos_get_side_to_move(pos);
        assert_true(side_to_move == BLACK);

        // make move quiet move and check en passent square is cleared
        pos_make_move(pos, quiet_move);
        enp_sq = pos_get_en_pass_sq(pos);
        assert_true(enp_sq == NO_SQUARE);

        pos_destroy(pos);
    }
}

void test_position_make_move_black_double_first_move(void **state) {
#define INITIAL_FEN_BLACK_TO_MOVE "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1\n"

    struct mv_from_to moves[8] = {{.from_sq = a7, .to_sq = a5}, {.from_sq = b7, .to_sq = b5},
                                  {.from_sq = c7, .to_sq = c5}, {.from_sq = d7, .to_sq = d5},
                                  {.from_sq = e7, .to_sq = e5}, {.from_sq = f7, .to_sq = f5},
                                  {.from_sq = g7, .to_sq = g5}, {.from_sq = h7, .to_sq = h5}};

    struct move quiet_move = move_encode_quiet(a2, a3);

    for (int i = 0; i < 8; i++) {
        struct position *pos = pos_create();
        pos_initialise(INITIAL_FEN_BLACK_TO_MOVE, pos);

        const enum square from_sq = moves[i].from_sq;
        const enum square to_sq = moves[i].to_sq;

        struct move mv = move_encode_pawn_double_first(from_sq, to_sq);

        // baseline, not set
        enum square enp_sq = pos_get_en_pass_sq(pos);
        assert_true(enp_sq = NO_SQUARE);

        // make move and check en passant square
        pos_make_move(pos, mv);
        enp_sq = pos_get_en_pass_sq(pos);
        assert_true(enp_sq != NO_SQUARE);

        const enum square expected_enp_sq = sq_get_square_minus_1_rank(from_sq);
        assert_true(expected_enp_sq == enp_sq);

        // check the pawn piece has moved
        bool is_from_sq_occupied = brd_is_sq_occupied(pos_get_board(pos), from_sq);
        assert_false(is_from_sq_occupied);
        bool is_to_sq_occupied = brd_is_sq_occupied(pos_get_board(pos), to_sq);
        assert_true(is_to_sq_occupied);

        // check the side has swapped
        enum colour side_to_move = pos_get_side_to_move(pos);
        assert_true(side_to_move == WHITE);

        // make a normal move, verify en passant square no longer active
        pos_make_move(pos, quiet_move);
        enp_sq = pos_get_en_pass_sq(pos);
        assert_true(enp_sq == NO_SQUARE);

        pos_destroy(pos);
    }
}

void test_position_make_move_castle_white_kingside_move_valid_position_updated(void **state) {
    const char *FEN = "r2qk2r/p1pp1p1p/bpn2np1/2b1p3/4P3/1PNPBN2/P1P1BPPP/"
                      "R2QK2R w KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);
    enum piece pce;

    const enum square start_rook_sq = h1;
    const enum square start_king_sq = e1;
    const enum square end_rook_sq = f1;
    const enum square end_king_sq = g1;

    // validate the starting position
    pce = brd_get_piece_on_square(pos_get_board(pos), start_king_sq);
    assert_true(pce_get_role(pce) == KING);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = brd_get_piece_on_square(pos_get_board(pos), start_rook_sq);
    assert_true(pce_get_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == WHITE);

    // validate initial castling permissions
    struct cast_perm_container start_cp = pos_get_cast_perm(pos);
    assert_true(cast_perm_has_white_kingside_permissions(start_cp));
    assert_true(cast_perm_has_white_queenside_permissions(start_cp));
    assert_true(cast_perm_has_black_kingside_permissions(start_cp));
    assert_true(cast_perm_has_black_queenside_permissions(start_cp));

    struct move wk_castle = move_encode_castle_kingside_white();

    // make move
    enum move_legality legality = pos_make_move(pos, wk_castle);
    assert_true(legality == LEGAL_MOVE);

    // verify end squares are as expected
    pce = brd_get_piece_on_square(pos_get_board(pos), end_rook_sq);
    assert_true(pce_get_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = brd_get_piece_on_square(pos_get_board(pos), end_king_sq);
    assert_true(pce_get_role(pce) == KING);
    assert_true(pce_get_colour(pce) == WHITE);

    // check castle permissions are updated
    struct cast_perm_container cp = pos_get_cast_perm(pos);
    assert_false(cast_perm_has_white_kingside_permissions(cp));
    assert_false(cast_perm_has_white_queenside_permissions(cp));
    assert_true(cast_perm_has_black_kingside_permissions(cp));
    assert_true(cast_perm_has_black_queenside_permissions(cp));
}

void test_position_make_move_castle_white_queenside_move_valid_position_updated(void **state) {
    const char *FEN = "r2qk2r/p1pp1p1p/bpn2np1/2b1p3/4P3/1PNPBN2/P1PQBPPP/"
                      "R3K2R w KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);
    enum piece pce;

    const enum square start_rook_sq = a1;
    const enum square start_king_sq = e1;
    const enum square end_rook_sq = d1;
    const enum square end_king_sq = c1;

    // validate the starting position
    pce = brd_get_piece_on_square(pos_get_board(pos), start_king_sq);
    assert_true(pce_get_role(pce) == KING);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = brd_get_piece_on_square(pos_get_board(pos), start_rook_sq);
    assert_true(pce_get_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == WHITE);

    // validate initial castling permissions
    struct cast_perm_container start_cp = pos_get_cast_perm(pos);
    assert_true(cast_perm_has_white_kingside_permissions(start_cp));
    assert_true(cast_perm_has_white_queenside_permissions(start_cp));
    assert_true(cast_perm_has_black_kingside_permissions(start_cp));
    assert_true(cast_perm_has_black_queenside_permissions(start_cp));

    struct move wk_castle = move_encode_castle_queenside_white();

    // make move
    enum move_legality legality = pos_make_move(pos, wk_castle);
    assert_true(legality == LEGAL_MOVE);

    // verify end squares are as expected
    pce = brd_get_piece_on_square(pos_get_board(pos), end_rook_sq);
    assert_true(pce_get_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = brd_get_piece_on_square(pos_get_board(pos), end_king_sq);
    assert_true(pce_get_role(pce) == KING);
    assert_true(pce_get_colour(pce) == WHITE);

    // check castle permissions are updated
    struct cast_perm_container cp = pos_get_cast_perm(pos);
    assert_false(cast_perm_has_white_kingside_permissions(cp));
    assert_false(cast_perm_has_white_queenside_permissions(cp));
    assert_true(cast_perm_has_black_kingside_permissions(cp));
    assert_true(cast_perm_has_black_queenside_permissions(cp));
}

void test_position_make_move_castle_black_queenside_move_valid_position_updated(void **state) {
    const char *FEN = "r3k2r/p1pp1p1p/bpn1qnp1/2b1p3/4P3/1PNPBN2/P1PQBPPP/"
                      "R3K2R b KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);
    enum piece pce;

    const enum square start_rook_sq = a8;
    const enum square start_king_sq = e8;
    const enum square end_rook_sq = d8;
    const enum square end_king_sq = c8;

    // validate the starting position
    pce = brd_get_piece_on_square(pos_get_board(pos), start_king_sq);
    assert_true(pce_get_role(pce) == KING);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = brd_get_piece_on_square(pos_get_board(pos), start_rook_sq);
    assert_true(pce_get_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == BLACK);

    assert_true(pos_get_side_to_move(pos) == BLACK);

    // validate initial castling permissions
    struct cast_perm_container start_cp = pos_get_cast_perm(pos);
    assert_true(cast_perm_has_white_kingside_permissions(start_cp));
    assert_true(cast_perm_has_white_queenside_permissions(start_cp));
    assert_true(cast_perm_has_black_kingside_permissions(start_cp));
    assert_true(cast_perm_has_black_queenside_permissions(start_cp));

    struct move bq_castle = move_encode_castle_queenside_black();

    // make move
    enum move_legality legality = pos_make_move(pos, bq_castle);
    assert_true(legality == LEGAL_MOVE);

    // verify end squares are as expected
    pce = brd_get_piece_on_square(pos_get_board(pos), end_rook_sq);
    assert_true(pce_get_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = brd_get_piece_on_square(pos_get_board(pos), end_king_sq);
    assert_true(pce_get_role(pce) == KING);
    assert_true(pce_get_colour(pce) == BLACK);

    // check castle permissions are updated
    struct cast_perm_container cp = pos_get_cast_perm(pos);
    assert_true(cast_perm_has_white_kingside_permissions(cp));
    assert_true(cast_perm_has_white_queenside_permissions(cp));
    assert_false(cast_perm_has_black_kingside_permissions(cp));
    assert_false(cast_perm_has_black_queenside_permissions(cp));
}

void test_position_make_move_castle_black_kingside_move_valid_position_updated(void **state) {
    const char *FEN = "r3k2r/p1ppqp1p/bpn2np1/2b1p3/4P3/1PNPBN2/P1PQBPPP/R3K2R "
                      "b KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);
    enum piece pce;

    const enum square start_rook_sq = h8;
    const enum square start_king_sq = e8;
    const enum square end_rook_sq = f8;
    const enum square end_king_sq = g8;

    // validate the starting position
    pce = brd_get_piece_on_square(pos_get_board(pos), start_king_sq);
    assert_true(pce_get_role(pce) == KING);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = brd_get_piece_on_square(pos_get_board(pos), start_rook_sq);
    assert_true(pce_get_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == BLACK);

    // validate initial castling permissions
    struct cast_perm_container start_cp = pos_get_cast_perm(pos);
    assert_true(cast_perm_has_white_kingside_permissions(start_cp));
    assert_true(cast_perm_has_white_queenside_permissions(start_cp));
    assert_true(cast_perm_has_black_kingside_permissions(start_cp));
    assert_true(cast_perm_has_black_queenside_permissions(start_cp));

    struct move bk_castle = move_encode_castle_kingside_black();

    // make move
    enum move_legality legality = pos_make_move(pos, bk_castle);
    assert_true(legality == LEGAL_MOVE);

    // verify end squares are as expected
    pce = brd_get_piece_on_square(pos_get_board(pos), end_rook_sq);
    assert_true(pce_get_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = brd_get_piece_on_square(pos_get_board(pos), end_king_sq);
    assert_true(pce_get_role(pce) == KING);
    assert_true(pce_get_colour(pce) == BLACK);

    // check castle permissions are updated
    struct cast_perm_container cp = pos_get_cast_perm(pos);
    assert_true(cast_perm_has_white_kingside_permissions(cp));
    assert_true(cast_perm_has_white_queenside_permissions(cp));
    assert_false(cast_perm_has_black_kingside_permissions(cp));
    assert_false(cast_perm_has_black_queenside_permissions(cp));
}

void test_position_make_move_castle_white_kingside_move_invalid(void **state) {

    const char *fen_list[] = {
        "4k3/4q3/8/8/8/8/8/R3K2R w K - 0 1", "4k3/5q2/8/8/8/8/8/R3K2R w K - 0 1", "4k3/6q1/8/8/8/8/8/R3K2R w K - 0 1",
        "4k3/8/8/8/8/3q4/8/R3K2R w K - 0 1", "4k3/8/8/8/8/7q/8/R3K2R w K - 0 1",  "4k3/8/8/8/8/8/3q4/R3K2R w K - 0 1",
        "4k3/8/q7/8/8/8/8/R3K2R w K - 0 1",  "4k3/8/8/q7/8/8/8/R3K2R w K - 0 1",  "4k3/q7/8/8/8/8/8/R3K2R w K - 0 1"};

    const uint8_t fen_sz = (sizeof(fen_list) / sizeof(const char *));
    struct move wk_castle = move_encode_castle_kingside_white();

    for (int i = 0; i < fen_sz; i++) {

        struct position *pos = pos_create();
        pos_initialise(fen_list[i], pos);

        struct board *brd = pos_get_board(pos);

        struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, wk_castle);

        struct material material_after = brd_get_material(brd);

        assert_int_equal(material_before.white, material_after.white);
        assert_int_equal(material_before.black, material_after.black);

        assert_true(legality == ILLEGAL_MOVE);

        pos_destroy(pos);
    }
}

void test_position_make_move_castle_black_kingside_move_invalid(void **state) {
    const char *fen_list[] = {"r3k2r/8/8/8/Q7/8/8/4K3 b k - 0 1",  "r3k2r/8/8/8/1Q6/8/8/4K3 b k - 0 1",
                              "r3k2r/8/8/8/2Q5/8/8/4K3 b k - 0 1", "r3k2r/8/8/8/4Q3/8/8/4K3 b k - 0 1",
                              "r3k2r/8/8/8/5Q2/8/8/4K3 b k - 0 1", "r3k2r/8/8/8/6Q1/8/8/4K3 b k - 0 1",
                              "r3k2r/8/6Q1/8/8/8/8/4K3 b k - 0 1", "r3k2r/8/7Q/8/8/8/8/4K3 b k - 0 1",
                              "r3k2r/7Q/8/8/8/8/8/4K3 b k - 0 1",  "r3k2r/6Q1/8/8/8/8/8/4K3 b k - 0 1"};

    const uint8_t fen_sz = (sizeof(fen_list) / sizeof(const char *));
    struct move bk_castle = move_encode_castle_kingside_black();

    for (int i = 0; i < fen_sz; i++) {

        struct position *pos = pos_create();
        pos_initialise(fen_list[i], pos);
        const struct board *brd = pos_get_board(pos);

        struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, bk_castle);

        struct material material_after = brd_get_material(brd);

        assert_int_equal(material_before.white, material_after.white);
        assert_int_equal(material_before.black, material_after.black);

        assert_true(legality == ILLEGAL_MOVE);

        pos_destroy(pos);
    }
}

void test_position_make_move_castle_white_queenside_move_invalid(void **state) {
    const char *fen_list[] = {
        "4k3/4q3/8/8/8/8/8/R3K2R w Q - 0 1", "4k3/8/3q4/8/8/8/8/R3K2R w Q - 0 1", "4k3/8/2q5/8/8/8/8/R3K2R w Q - 0 1",
        "4k3/8/8/8/8/1q6/8/R3K2R w Q - 0 1", "4k3/8/8/8/8/8/1q6/R3K2R w Q - 0 1", "4k3/8/8/8/8/5q2/8/R3K2R w Q - 0 1",
        "4k3/8/8/8/5q2/8/8/R3K2R w Q - 0 1", "4k3/8/8/7q/8/8/8/R3K2R w Q - 0 1",  "4k3/8/8/8/8/4q3/8/R3K2R w Q - 0 1",
    };

    const uint8_t fen_sz = (sizeof(fen_list) / sizeof(const char *));
    struct move wq_castle = move_encode_castle_queenside_white();

    for (int i = 0; i < fen_sz; i++) {

        struct position *pos = pos_create();
        pos_initialise(fen_list[i], pos);
        const struct board *brd = pos_get_board(pos);

        struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, wq_castle);

        assert_true(legality == ILLEGAL_MOVE);
        struct material material_after = brd_get_material(brd);

        assert_int_equal(material_before.white, material_after.white);
        assert_int_equal(material_before.black, material_after.black);

        pos_destroy(pos);
    }
}

void test_position_make_move_castle_black_queenside_move_invalid(void **state) {

    const char *fen_list[] = {"r3k2r/8/8/8/Q7/8/8/4K3 b q - 0 1",  "r3k2r/8/8/8/2Q5/8/8/4K3 b q - 0 1",
                              "r3k2r/8/8/8/3Q4/8/8/4K3 b q - 0 1", "r3k2r/8/8/8/4Q3/8/8/4K3 b q - 0 1",
                              "r3k2r/8/6Q1/8/8/8/8/4K3 b q - 0 1", "r3k2r/8/5Q2/8/8/8/8/4K3 b q - 0 1",
                              "r3k2r/8/3Q4/8/8/8/8/4K3 b q - 0 1"};

    const uint8_t fen_sz = (sizeof(fen_list) / sizeof(const char *));
    struct move bq_castle = move_encode_castle_queenside_black();

    for (int i = 0; i < fen_sz; i++) {

        struct position *pos = pos_create();
        pos_initialise(fen_list[i], pos);
        const struct board *brd = pos_get_board(pos);

        struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, bq_castle);

        assert_true(legality == ILLEGAL_MOVE);
        struct material material_after = brd_get_material(brd);

        assert_int_equal(material_before.white, material_after.white);
        assert_int_equal(material_before.black, material_after.black);

        pos_destroy(pos);
    }
}

void test_position_brd_is_sq_occupied(void **state) {
    const char *FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/"
                      "1bNnrQ2 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    struct board *brd = pos_get_board(pos);

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

void test_position_make_move_black_knight(void **state) {
    const char *test_fen = "5N2/B7/3n1k2/pP1K3B/2P5/1b3p1P/2p1n1pP/N1b5 b - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(e2, d4),   move_encode_quiet(e2, c3),   move_encode_quiet(e2, g1),
        move_encode_quiet(e2, g3),   move_encode_quiet(e2, f4),

        move_encode_quiet(d6, b7),   move_encode_quiet(d6, c8),   move_encode_quiet(d6, e8),
        move_encode_quiet(d6, f7),   move_encode_quiet(d6, f5),   move_encode_quiet(d6, e4),

        move_encode_capture(d6, b5), move_encode_capture(d6, c4),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        const enum square from_sq = move_decode_from_sq(mv);
        const enum square to_sq = move_decode_to_sq(mv);

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);
        const struct board *brd = pos_get_board(pos);

        const struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, mv);

        const struct material material_after = brd_get_material(brd);

        assert_true(legality == LEGAL_MOVE);
        if (move_is_quiet(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_int_equal(material_before.black, material_after.black);
        } else if (move_is_capture(mv)) {
            assert_int_equal(material_before.black, material_after.black);
            assert_true(material_before.white > material_after.white);
        }

        assert_false(brd_is_sq_occupied(brd, from_sq));
        enum piece pce = brd_get_piece_on_square(brd, to_sq);
        assert_true(pce == BLACK_KNIGHT);

        pos_destroy(pos);
    }
}

void test_position_make_move_white_knight(void **state) {
    const char *test_fen = "5N2/B7/5k2/pP1K3B/2P5/1b3pnP/n1p3pP/N1b5 w - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(f8, d7), move_encode_quiet(f8, e6),   move_encode_quiet(f8, g6),
        move_encode_quiet(f8, h7), move_encode_capture(a1, b3), move_encode_capture(a1, c2),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        const enum square from_sq = move_decode_from_sq(mv);
        const enum square to_sq = move_decode_to_sq(mv);

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);
        const struct board *brd = pos_get_board(pos);

        const struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, mv);

        const struct material material_after = brd_get_material(brd);

        assert_true(legality == LEGAL_MOVE);
        if (move_is_quiet(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_int_equal(material_before.black, material_after.black);
        } else if (move_is_capture(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_true(material_before.black > material_after.black);
        }

        assert_false(brd_is_sq_occupied(brd, from_sq));
        enum piece pce = brd_get_piece_on_square(brd, to_sq);
        assert_true(pce == WHITE_KNIGHT);

        pos_destroy(pos);
    }
}

void test_position_make_move_black_bishop(void **state) {
    const char *test_fen = "6K1/4k3/1Q6/p6B/P1P1BP2/1bp2p2/3b2pP/8 b - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(b3, a2),   move_encode_quiet(b3, c2),   move_encode_quiet(b3, d1),
        move_encode_quiet(d2, e1),   move_encode_quiet(d2, e3),   move_encode_quiet(d2, c1),

        move_encode_capture(b3, a4), move_encode_capture(b3, c4), move_encode_capture(d2, f4),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        const enum square from_sq = move_decode_from_sq(mv);
        const enum square to_sq = move_decode_to_sq(mv);

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);
        const struct board *brd = pos_get_board(pos);

        const struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, mv);

        const struct material material_after = brd_get_material(brd);

        assert_true(legality == LEGAL_MOVE);
        if (move_is_quiet(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_int_equal(material_before.black, material_after.black);
        } else if (move_is_capture(mv)) {
            assert_int_equal(material_before.black, material_after.black);
            assert_true(material_before.white > material_after.white);
        }

        assert_false(brd_is_sq_occupied(brd, from_sq));
        enum piece pce = brd_get_piece_on_square(brd, to_sq);
        assert_true(pce == BLACK_BISHOP);

        pos_destroy(pos);
    }
}

void test_position_make_move_white_bishop(void **state) {
    const char *test_fen = "6K1/8/1Q3k2/p6B/P1P1BP2/1b3p2/2p3pP/4b3 w - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(e4, d3),   move_encode_quiet(e4, d5),   move_encode_quiet(e4, d6),
        move_encode_quiet(e4, b7),   move_encode_quiet(e4, a8),   move_encode_quiet(e4, f5),
        move_encode_quiet(e4, g6),   move_encode_quiet(e4, h7),   move_encode_quiet(h5, g4),
        move_encode_quiet(h5, g6),   move_encode_quiet(h5, f7),   move_encode_quiet(h5, e8),

        move_encode_capture(e4, c2), move_encode_capture(e4, f3), move_encode_capture(h5, f3),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        const enum square from_sq = move_decode_from_sq(mv);
        const enum square to_sq = move_decode_to_sq(mv);

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);
        const struct board *brd = pos_get_board(pos);

        const struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, mv);

        const struct material material_after = brd_get_material(brd);

        assert_true(legality == LEGAL_MOVE);
        if (move_is_quiet(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_int_equal(material_before.black, material_after.black);
        } else if (move_is_capture(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_true(material_before.black > material_after.black);
        }

        assert_false(brd_is_sq_occupied(brd, from_sq));
        enum piece pce = brd_get_piece_on_square(brd, to_sq);
        assert_true(pce == WHITE_BISHOP);

        pos_destroy(pos);
    }
}

void test_position_make_move_black_queen(void **state) {
    const char *test_fen = "7k/8/1Q5b/p4q1B/P1P1BP2/1bp2p2/6pP/2K5 b - - 0 1n";

    struct move mv_list[] = {
        move_encode_quiet(f5, g5),   move_encode_quiet(f5, e5),

        move_encode_quiet(f5, d5),   move_encode_quiet(f5, c5),   move_encode_quiet(f5, b5),
        move_encode_quiet(f5, g4),   move_encode_quiet(f5, h3),   move_encode_quiet(f5, g6),

        move_encode_quiet(f5, h7),   move_encode_quiet(f5, f6),   move_encode_quiet(f5, f7),
        move_encode_quiet(f5, f8),

        move_encode_quiet(f5, e6),   move_encode_quiet(f5, d7),   move_encode_quiet(f5, c8),

        move_encode_capture(f5, h5), move_encode_capture(f5, f4), move_encode_capture(f5, e4),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        const enum square from_sq = move_decode_from_sq(mv);
        const enum square to_sq = move_decode_to_sq(mv);

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);
        const struct board *brd = pos_get_board(pos);

        const struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, mv);

        const struct material material_after = brd_get_material(brd);

        assert_true(legality == LEGAL_MOVE);
        if (move_is_quiet(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_int_equal(material_before.black, material_after.black);
        } else if (move_is_capture(mv)) {
            assert_int_equal(material_before.black, material_after.black);
            assert_true(material_before.white > material_after.white);
        }

        assert_false(brd_is_sq_occupied(brd, from_sq));
        enum piece pce = brd_get_piece_on_square(brd, to_sq);
        assert_true(pce == BLACK_QUEEN);

        pos_destroy(pos);
    }
}

void test_position_make_move_white_queen(void **state) {
    const char *test_fen = "7k/8/1Q5b/p4q1B/P1P1BP2/1bp2p2/6pP/2K5 w - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(b6, a6),   move_encode_quiet(b6, c6),   move_encode_quiet(b6, d6),
        move_encode_quiet(b6, e6),   move_encode_quiet(b6, f6),   move_encode_quiet(b6, g6),
        move_encode_quiet(b6, b5),   move_encode_quiet(b6, b4),   move_encode_quiet(b6, b7),
        move_encode_quiet(b6, b8),   move_encode_quiet(b6, a7),   move_encode_quiet(b6, c7),
        move_encode_quiet(b6, d8),   move_encode_quiet(b6, c5),   move_encode_quiet(b6, d4),
        move_encode_quiet(b6, e3),   move_encode_quiet(b6, f2),   move_encode_quiet(b6, g1),

        move_encode_capture(b6, a5), move_encode_capture(b6, h6), move_encode_capture(b6, b3),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        const enum square from_sq = move_decode_from_sq(mv);
        const enum square to_sq = move_decode_to_sq(mv);

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);
        const struct board *brd = pos_get_board(pos);

        const struct material material_before = brd_get_material(brd);

        enum move_legality legality = pos_make_move(pos, mv);

        const struct material material_after = brd_get_material(brd);

        assert_true(legality == LEGAL_MOVE);
        if (move_is_quiet(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_int_equal(material_before.black, material_after.black);
        } else if (move_is_capture(mv)) {
            assert_int_equal(material_before.white, material_after.white);
            assert_true(material_before.black > material_after.black);
        }

        assert_false(brd_is_sq_occupied(brd, from_sq));
        enum piece pce = brd_get_piece_on_square(brd, to_sq);
        assert_true(pce == WHITE_QUEEN);

        pos_destroy(pos);
    }
}

void test_position_make_move_white_discovered_attack_on_king_invalid_move(void **state) {
    const char *test_fen = "7k/8/1Q5b/p4q1B/P1P2P2/1bp2p2/2B3pP/3K4 w - - 0 1\n";

    const struct move mv = move_encode_quiet(c2, b1);
    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == ILLEGAL_MOVE);

    pos_destroy(pos);
}

void test_position_make_move_black_discovered_attack_on_king_invalid_move(void **state) {
    const char *test_fen = "Q4b1k/8/8/p4q1B/P1P1BP2/1bp2p2/6pP/6K1 b - - 0 1\n";

    const struct move mv = move_encode_quiet(f8, a3);
    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == ILLEGAL_MOVE);

    pos_destroy(pos);
}

void test_position_make_move_black_king_valid_moves(void **state) {
    const char *test_fen = "3b2k1/Q7/8/p4q1B/P1P1BP2/1bp2p2/6pP/6K1 b - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(g8, h8),
        move_encode_quiet(g8, f8),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        const enum square from_sq = move_decode_from_sq(mv);
        const enum square to_sq = move_decode_to_sq(mv);

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);
        const struct board *brd = pos_get_board(pos);

        enum move_legality legality = pos_make_move(pos, mv);

        assert_true(legality == LEGAL_MOVE);

        assert_false(brd_is_sq_occupied(brd, from_sq));
        enum piece pce = brd_get_piece_on_square(brd, to_sq);
        assert_true(pce == BLACK_KING);

        pos_destroy(pos);
    }
}

void test_position_make_move_black_king_invalid_moves(void **state) {
    const char *test_fen = "3b2k1/Q7/8/p4q1B/P1P1BP2/1bp2p2/6pP/6K1 b - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(g8, f7),
        move_encode_quiet(g8, g7),
        move_encode_quiet(g8, h7),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        enum move_legality legality = pos_make_move(pos, mv);

        assert_true(legality == ILLEGAL_MOVE);

        pos_destroy(pos);
    }
}

void test_position_make_move_white_king_valid_moves(void **state) {
    const char *test_fen = "3b2K1/2q5/8/p6B/P1P1BQ2/1bp2p2/6pP/3k4 w - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(g8, h8),
        move_encode_quiet(g8, f8),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        const enum square from_sq = move_decode_from_sq(mv);
        const enum square to_sq = move_decode_to_sq(mv);

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);
        const struct board *brd = pos_get_board(pos);

        enum move_legality legality = pos_make_move(pos, mv);

        assert_true(legality == LEGAL_MOVE);

        assert_false(brd_is_sq_occupied(brd, from_sq));
        enum piece pce = brd_get_piece_on_square(brd, to_sq);
        assert_true(pce == WHITE_KING);

        pos_destroy(pos);
    }
}

void test_position_make_move_white_king_invalid_moves(void **state) {
    const char *test_fen = "3b2K1/2q5/8/p6B/P1P1BQ2/1bp2p2/6pP/3k4 w - - 0 1\n";

    struct move mv_list[] = {
        move_encode_quiet(g8, f7),
        move_encode_quiet(g8, g7),
        move_encode_quiet(g8, h7),
    };

    const uint8_t mv_sz = (sizeof(mv_list) / sizeof(struct move));
    for (int i = 0; i < mv_sz; i++) {
        const struct move mv = mv_list[i];
        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        enum move_legality legality = pos_make_move(pos, mv);

        assert_true(legality == ILLEGAL_MOVE);

        pos_destroy(pos);
    }
}

void test_position_make_move_black_en_passant(void **state) {
    const char *test_fen = "4k3/8/8/8/1p6/8/2P5/4K3 w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);
    struct board *brd = pos_get_board(pos);

    // double first move
    const struct move mv = move_encode_pawn_double_first(c2, c4);
    enum move_legality legality = pos_make_move(pos, mv);

    assert_true(legality == LEGAL_MOVE);
    enum piece old_white_pawn = brd_get_piece_on_square(brd, c4);
    assert_true(pce_get_role(old_white_pawn) == PAWN);
    assert_true(pce_get_colour(old_white_pawn) == WHITE);
    enum square enp_sq = pos_get_en_pass_sq(pos);
    assert_true(enp_sq == c3);

    // en passant move
    const struct move en_pass_mv = move_encode_enpassant(b4, c3);
    legality = pos_make_move(pos, en_pass_mv);

    assert_false(brd_is_sq_occupied(brd, c4));

    assert_true(brd_is_sq_occupied(brd, c3));
    enum piece blk_pawn = brd_get_piece_on_square(brd, c3);
    assert_true(blk_pawn == BLACK_PAWN);
    enp_sq = pos_get_en_pass_sq(pos);
    assert_true(enp_sq == NO_SQUARE);

    pos_destroy(pos);
}

void test_position_make_move_white_en_passant(void **state) {
    const char *test_fen = "4k3/6p1/8/5P2/8/8/8/4K3 b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);
    struct board *brd = pos_get_board(pos);

    // double first move
    const struct move mv = move_encode_pawn_double_first(g7, g5);
    enum move_legality legality = pos_make_move(pos, mv);

    assert_true(legality == LEGAL_MOVE);
    enum piece old_black_pawn = brd_get_piece_on_square(brd, g5);
    assert_true(pce_get_role(old_black_pawn) == PAWN);
    assert_true(pce_get_colour(old_black_pawn) == BLACK);
    enum square enp_sq = pos_get_en_pass_sq(pos);
    assert_true(enp_sq == g6);

    // en passant move
    const struct move en_pass_mv = move_encode_enpassant(f5, g6);
    legality = pos_make_move(pos, en_pass_mv);

    assert_false(brd_is_sq_occupied(brd, g5));

    enum piece white_pawn = brd_get_piece_on_square(brd, g6);
    assert_true(white_pawn == WHITE_PAWN);
    enp_sq = pos_get_en_pass_sq(pos);
    assert_true(enp_sq == NO_SQUARE);

    pos_destroy(pos);
}

void test_position_make_move_white_promotion(void **state) {
    const char *test_fen = "4k3/1P6/8/8/8/8/8/4K3 w - - 0 1\n";

    struct decode_move mv_list[] = {
        {.mv = move_encode_promote_knight(b7, b8), .decoded_piece = WHITE_KNIGHT},
        {.mv = move_encode_promote_bishop(b7, b8), .decoded_piece = WHITE_BISHOP},
        {.mv = move_encode_promote_rook(b7, b8), .decoded_piece = WHITE_ROOK},
        {.mv = move_encode_promote_queen(b7, b8), .decoded_piece = WHITE_QUEEN},
    };

    for (int i = 0; i < 4; i++) {
        const struct move mv = mv_list[i].mv;
        enum piece expected_prom_pce = mv_list[i].decoded_piece;

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        const struct board *brd = pos_get_board(pos);

        assert_true(brd_is_sq_occupied(brd, b7));
        assert_false(brd_is_sq_occupied(brd, b8));

        enum move_legality legality = pos_make_move(pos, mv);

        assert_true(legality == LEGAL_MOVE);
        enum piece prom_pce = brd_get_piece_on_square(brd, b8);
        assert_true(prom_pce == expected_prom_pce);
        assert_false(brd_is_sq_occupied(brd, b7));

        pos_destroy(pos);
    }
}

void test_position_make_move_black_promotion(void **state) {
    const char *test_fen = "4k3/8/8/8/8/8/5p2/1K6 b - - 0 1\n";

    struct decode_move mv_list[] = {
        {.mv = move_encode_promote_knight(f2, f1), .decoded_piece = BLACK_KNIGHT},
        {.mv = move_encode_promote_bishop(f2, f1), .decoded_piece = BLACK_BISHOP},
        {.mv = move_encode_promote_rook(f2, f1), .decoded_piece = BLACK_ROOK},
        {.mv = move_encode_promote_queen(f2, f1), .decoded_piece = BLACK_QUEEN},
    };

    for (int i = 0; i < 4; i++) {
        const struct move mv = mv_list[i].mv;
        enum piece expected_prom_pce = mv_list[i].decoded_piece;

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        const struct board *brd = pos_get_board(pos);

        assert_true(brd_is_sq_occupied(brd, f2));
        assert_false(brd_is_sq_occupied(brd, f1));

        enum move_legality legality = pos_make_move(pos, mv);

        assert_true(legality == LEGAL_MOVE);
        enum piece prom_pce = brd_get_piece_on_square(brd, f1);
        assert_true(prom_pce == expected_prom_pce);
        assert_false(brd_is_sq_occupied(brd, f2));

        pos_destroy(pos);
    }
}

void test_position_make_move_white_promotion_capture(void **state) {
    const char *test_fen = "2r1k3/1P6/8/8/8/8/8/1K6 w - - 0 1\n";

    struct decode_move mv_list[] = {
        {.mv = move_encode_promote_knight_with_capture(b7, c8), .decoded_piece = WHITE_KNIGHT},
        {.mv = move_encode_promote_bishop_with_capture(b7, c8), .decoded_piece = WHITE_BISHOP},
        {.mv = move_encode_promote_rook_with_capture(b7, c8), .decoded_piece = WHITE_ROOK},
        {.mv = move_encode_promote_queen_with_capture(b7, c8), .decoded_piece = WHITE_QUEEN},
    };

    for (int i = 0; i < 4; i++) {
        const struct move mv = mv_list[i].mv;
        enum piece expected_prom_pce = mv_list[i].decoded_piece;

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        const struct board *brd = pos_get_board(pos);

        assert_true(brd_is_sq_occupied(brd, b7));
        assert_true(brd_is_sq_occupied(brd, c8));

        enum move_legality legality = pos_make_move(pos, mv);

        assert_true(legality == LEGAL_MOVE);
        enum piece prom_pce = brd_get_piece_on_square(brd, c8);
        assert_true(prom_pce == expected_prom_pce);
        assert_false(brd_is_sq_occupied(brd, b7));

        pos_destroy(pos);
    }
}

void test_position_make_move_black_promotion_capture(void **state) {
    const char *test_fen = "4k3/8/8/8/8/8/4p3/1K3B2 b - - 0 1\n";

    struct decode_move mv_list[] = {
        {.mv = move_encode_promote_knight_with_capture(e2, f1), .decoded_piece = BLACK_KNIGHT},
        {.mv = move_encode_promote_bishop_with_capture(e2, f1), .decoded_piece = BLACK_BISHOP},
        {.mv = move_encode_promote_rook_with_capture(e2, f1), .decoded_piece = BLACK_ROOK},
        {.mv = move_encode_promote_queen_with_capture(e2, f1), .decoded_piece = BLACK_QUEEN},
    };

    for (int i = 0; i < 4; i++) {
        const struct move mv = mv_list[i].mv;
        enum piece expected_prom_pce = mv_list[i].decoded_piece;

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        const struct board *brd = pos_get_board(pos);

        assert_true(brd_is_sq_occupied(brd, e2));
        assert_true(brd_is_sq_occupied(brd, f1));

        enum move_legality legality = pos_make_move(pos, mv);

        assert_true(legality == LEGAL_MOVE);
        enum piece prom_pce = brd_get_piece_on_square(brd, f1);
        assert_true(prom_pce == expected_prom_pce);
        assert_false(brd_is_sq_occupied(brd, e2));

        pos_destroy(pos);
    }
}

void test_position_make_move_then_take_move_positions_restored_as_expected(void **state) {
    const char *test_fen = "4k3/6p1/8/5P2/8/8/8/4K3 b - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    // make a similar position for comparison
    struct position *pos_original = pos_create();
    pos_initialise(test_fen, pos_original);

    // confirm they're the same before testing
    assert_true(pos_compare(pos, pos_original));

    const struct move mv = move_encode_pawn_double_first(g7, g5);
    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    // revert the move
    pos_take_move(pos);

    assert_true(pos_compare(pos, pos_original));

    pos_destroy(pos);
    pos_destroy(pos_original);
}

void test_position_make_move_sparse_board_black_to_move(void **state) {
    const char *test_fen = "8/8/8/8/8/8/1k6/R3K3 b Q - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const struct move mv = move_encode_capture(b2, a1);
    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_destroy(pos);
}

void test_position_make_take_double_pawn_move_white(void **state) {
    const char *fen = "4k3/8/8/8/1p6/8/2P5/4K3 w - c3 0 1\n";

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    const struct move mv = move_encode_pawn_double_first(c2, c4);
    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_quiet_move_white(void **state) {
    const char *fen = "4k3/8/2n5/8/1p6/5N2/2P5/4K3 w - c3 0 1\n";

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    const struct move mv = move_encode_quiet(f3, g5);
    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_double_pawn_move_black(void **state) {
    const char *fen = "4k3/1p6/2n5/8/8/5N2/2P5/4K3 b - c3 0 1\n";

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    const struct move mv = move_encode_pawn_double_first(b7, b5);
    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_quiet_move_black(void **state) {
    const char *fen = "4k3/1p6/5n2/8/8/5N2/2P5/4K3 b - c3 0 1\n";

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    const struct move mv = move_encode_quiet(f6, e4);
    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_capture_move_white(void **state) {
    const char *fen = "4k3/1p6/5n2/2N5/8/8/2P5/4K3 w - c3 0 1\n";

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    const struct move mv = move_encode_capture(c5, b7);
    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_capture_move_black(void **state) {
    const char *fen = "4k3/1p6/8/2N5/8/4n3/2P5/4K3 b - c3 0 1\n";

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    const struct move mv = move_encode_capture(e3, c2);
    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_en_passant_white_capture(void **state) {
    const char *fen = "4k3/8/8/5Pp1/8/8/8/4K3 w - g6 0 1\n";
    const struct move mv = move_encode_enpassant(f5, g6);

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_en_passant_black_capture(void **state) {
    const char *fen = "4k3/8/8/8/5Pp1/8/8/4K3 b - f3 0 1\n";
    const struct move mv = move_encode_enpassant(g4, f3);

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_queen_castle_white(void **state) {
    const char *fen = "r2qk2r/p1pp1p1p/bpn2np1/2b1p3/4P3/1PNPBN2/P1PQBPPP/R3K2R w KQkq - 0 1\n";
    const struct move mv = move_encode_castle_queenside_white();

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_king_castle_white(void **state) {
    const char *fen = "r2qk2r/p1pp1p1p/bpn2np1/2b1p3/4P3/1PNPBN2/P1PQBPPP/R3K2R w KQkq - 0 1\n";
    const struct move mv = move_encode_castle_kingside_white();

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_queen_castle_black(void **state) {
    const char *fen = "r3k2r/p1pp1p1p/bpn1qnp1/2b1p3/4P3/1PNPBN2/P1PQBPPP/R3K2R b KQkq - 0 1\n";
    const struct move mv = move_encode_castle_queenside_black();

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_king_castle_black(void **state) {
    const char *fen = "r2qk2r/p1pp1p1p/bpn2np1/2b1p3/4P3/1PNPBN2/P1PQBPPP/R3K2R b KQkq - 0 1\n";
    const struct move mv = move_encode_castle_kingside_black();

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_quiet_promotion_white(void **state) {
    const char *fen = "8/2P5/8/7k/1p6/8/8/4K3 w - - 0 1\n";
    const struct move mv = move_encode_promote_bishop(c7, c8);

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_quiet_promotion_black(void **state) {
    const char *fen = "8/8/8/7k/2P5/8/1p6/4K3 b - - 0 1\n";
    const struct move mv = move_encode_promote_bishop(b2, b1);

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_capture_promotion_white(void **state) {
    const char *fen = "3n4/2P5/8/7k/8/8/1p6/4K3 w - - 0 1\n";
    const struct move mv = move_encode_promote_bishop_with_capture(c7, d8);

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_make_take_capture_promotion_black(void **state) {
    const char *fen = "8/2P5/8/7k/8/8/1p6/2N1K3 b - - 0 1\n";
    const struct move mv = move_encode_promote_bishop_with_capture(b2, c1);

    // initial position for comparison
    struct position *init_pos = pos_create();
    pos_initialise(fen, init_pos);

    struct position *pos = pos_create();
    pos_initialise(fen, pos);

    enum move_legality legality = pos_make_move(pos, mv);
    assert_true(legality == LEGAL_MOVE);

    pos_take_move(pos);

    assert_true(pos_compare(init_pos, pos));

    pos_destroy(init_pos);
    pos_destroy(pos);
}

void test_position_hash_updated_white_pawn_quiet_move(void **state) {
    const char *FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/"
                      "1bNnrQ2 w - - 0 1\n";

    const enum square FROM_SQ = g5;
    const enum square TO_SQ = g6;

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);
    const struct move pawn_move = move_encode_quiet(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update_move(WHITE_PAWN, FROM_SQ, TO_SQ, orig_pos_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, pawn_move);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));
}

void test_position_hash_updated_black_pawn_quiet_move(void **state) {
    const char *FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/"
                      "1bNnrQ2 b - - 0 1\n";

    const enum square FROM_SQ = d5;
    const enum square TO_SQ = d4;

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);
    const struct move pawn_move = move_encode_quiet(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update_move(BLACK_PAWN, FROM_SQ, TO_SQ, orig_pos_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, pawn_move);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));
}

void test_position_hash_updated_white_pawn_capture_move(void **state) {
    const char *FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/"
                      "1bNnrQ2 w - - 0 1\n";

    const enum square FROM_SQ = d3;
    const enum square TO_SQ = c4;

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);
    const struct move pawn_move = move_encode_capture(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update(BLACK_PAWN, TO_SQ, orig_pos_hash);
    expected_hash = hash_piece_update_move(WHITE_PAWN, FROM_SQ, TO_SQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, pawn_move);
    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));
}

void test_position_hash_updated_black_pawn_capture_move(void **state) {
    const char *FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/"
                      "1bNnrQ2 b - - 0 1\n";

    const enum square FROM_SQ = h6;
    const enum square TO_SQ = g5;

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);
    const struct move pawn_move = move_encode_capture(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update(WHITE_PAWN, TO_SQ, orig_pos_hash);
    expected_hash = hash_piece_update_move(BLACK_PAWN, FROM_SQ, TO_SQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, pawn_move);
    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));
}

void test_position_hash_updated_white_pawn_double_first_move(void **state) {
    const char *FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n";

    const enum square FROM_SQ = d2;
    const enum square TO_SQ = d4;

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);
    const struct move pawn_move = move_encode_pawn_double_first(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update_move(WHITE_PAWN, FROM_SQ, TO_SQ, orig_pos_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, pawn_move);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));
}

void test_position_hash_updated_black_pawn_double_first_move(void **state) {
    const char *FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1\n";

    const enum square FROM_SQ = d7;
    const enum square TO_SQ = d5;

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);
    const struct move pawn_move = move_encode_pawn_double_first(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update_move(BLACK_PAWN, FROM_SQ, TO_SQ, orig_pos_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, pawn_move);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));
}

void test_position_hash_updated_white_en_passant(void **state) {
    const char *test_fen = "4k3/8/8/5Pp1/8/8/8/4K3 w - g6 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    // en passant move
    const enum square FROM_SQ = f5;
    const enum square TO_SQ = g6;
    const enum square EN_PASS_SQ = TO_SQ;
    const enum square BLK_PAWN_SQ = g5;
    const struct move en_pass_mv = move_encode_enpassant(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update(BLACK_PAWN, BLK_PAWN_SQ, orig_pos_hash);
    expected_hash = hash_piece_update_move(WHITE_PAWN, FROM_SQ, TO_SQ, expected_hash);
    expected_hash = hash_en_passant(EN_PASS_SQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, en_pass_mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_black_en_passant(void **state) {
    const char *test_fen = "4k3/8/8/8/1pP5/8/8/4K3 b - c3 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    // en passant move
    const enum square FROM_SQ = b4;
    const enum square TO_SQ = c3;
    const enum square EN_PASS_SQ = TO_SQ;
    const enum square WHITE_PAWN_SQ = c4;
    const struct move en_pass_mv = move_encode_enpassant(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update(WHITE_PAWN, WHITE_PAWN_SQ, orig_pos_hash);
    expected_hash = hash_piece_update_move(BLACK_PAWN, FROM_SQ, TO_SQ, expected_hash);
    expected_hash = hash_en_passant(EN_PASS_SQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, en_pass_mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_white_promotion_quiet(void **state) {
    const char *test_fen = "8/2P5/8/7k/1p6/8/8/4K3 w - - 0 1\n";
    const enum square FROM_SQ = c7;
    const enum square TO_SQ = c8;

    struct move mv_list[] = {
        move_encode_promote_queen(FROM_SQ, TO_SQ),
        move_encode_promote_knight(FROM_SQ, TO_SQ),
        move_encode_promote_rook(FROM_SQ, TO_SQ),
        move_encode_promote_bishop(FROM_SQ, TO_SQ),
    };

    const enum piece prom_pces[] = {WHITE_QUEEN, WHITE_KNIGHT, WHITE_ROOK, WHITE_BISHOP};

    for (int i = 0; i < 4; i++) {
        const enum piece PROMOTION_PCE = prom_pces[i];

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        const uint64_t orig_pos_hash = pos_get_hash(pos);
        assert_true(orig_pos_hash != 0);

        const struct move mv = mv_list[i];

        uint64_t expected_hash = hash_piece_update(WHITE_PAWN, FROM_SQ, orig_pos_hash);
        expected_hash = hash_piece_update(PROMOTION_PCE, TO_SQ, expected_hash);
        expected_hash = hash_side_update(expected_hash);

        pos_make_move(pos, mv);

        const uint64_t after_move_pos_hash = pos_get_hash(pos);
        assert_true(after_move_pos_hash != 0);
        assert_true(after_move_pos_hash == expected_hash);
        assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

        pos_destroy(pos);
    }
}

void test_position_hash_updated_black_promotion_quiet(void **state) {
    const char *test_fen = "8/8/8/7k/2P5/5K2/1p6/8 b - - 0 1\n";
    const enum square FROM_SQ = b2;
    const enum square TO_SQ = b1;

    struct move mv_list[] = {
        move_encode_promote_queen(FROM_SQ, TO_SQ),
        move_encode_promote_knight(FROM_SQ, TO_SQ),
        move_encode_promote_rook(FROM_SQ, TO_SQ),
        move_encode_promote_bishop(FROM_SQ, TO_SQ),
    };

    const enum piece prom_pces[] = {BLACK_QUEEN, BLACK_KNIGHT, BLACK_ROOK, BLACK_BISHOP};

    for (int i = 0; i < 4; i++) {
        const enum piece PROMOTION_PCE = prom_pces[i];

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        const uint64_t orig_pos_hash = pos_get_hash(pos);
        assert_true(orig_pos_hash != 0);

        const struct move mv = mv_list[i];

        uint64_t expected_hash = hash_piece_update(BLACK_PAWN, FROM_SQ, orig_pos_hash);
        expected_hash = hash_piece_update(PROMOTION_PCE, TO_SQ, expected_hash);
        expected_hash = hash_side_update(expected_hash);

        pos_make_move(pos, mv);

        const uint64_t after_move_pos_hash = pos_get_hash(pos);
        assert_true(after_move_pos_hash != 0);
        assert_true(after_move_pos_hash == expected_hash);
        assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

        pos_destroy(pos);
    }
}

void test_position_hash_updated_white_promotion_capture(void **state) {
    const char *test_fen = "3n4/2P5/8/7k/1p6/5K2/8/8 w - - 0 1\n";
    const enum square FROM_SQ = c7;
    const enum square TO_SQ = d8;

    struct move

        mv_list[] = {
            move_encode_promote_queen_with_capture(FROM_SQ, TO_SQ),
            move_encode_promote_knight_with_capture(FROM_SQ, TO_SQ),
            move_encode_promote_rook_with_capture(FROM_SQ, TO_SQ),
            move_encode_promote_bishop_with_capture(FROM_SQ, TO_SQ),
        };

    const enum piece prom_pces[] = {WHITE_QUEEN, WHITE_KNIGHT, WHITE_ROOK, WHITE_BISHOP};

    for (int i = 0; i < 4; i++) {
        const enum piece PROMOTION_PCE = prom_pces[i];

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        const uint64_t orig_pos_hash = pos_get_hash(pos);
        assert_true(orig_pos_hash != 0);

        const struct move mv = mv_list[i];

        uint64_t expected_hash = hash_piece_update(WHITE_PAWN, FROM_SQ, orig_pos_hash);
        expected_hash = hash_piece_update(BLACK_KNIGHT, TO_SQ, expected_hash);
        expected_hash = hash_piece_update(PROMOTION_PCE, TO_SQ, expected_hash);
        expected_hash = hash_side_update(expected_hash);

        pos_make_move(pos, mv);

        const uint64_t after_move_pos_hash = pos_get_hash(pos);
        assert_true(after_move_pos_hash != 0);
        assert_true(after_move_pos_hash == expected_hash);
        assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

        pos_destroy(pos);
    }
}

void test_position_hash_updated_black_promotion_capture(void **state) {
    const char *test_fen = "38/8/8/2P4k/8/5K2/2p5/3N4 b - - 0 1\n";
    const enum square FROM_SQ = c2;
    const enum square TO_SQ = d1;

    struct move mv_list[] = {
        move_encode_promote_queen_with_capture(FROM_SQ, TO_SQ),
        move_encode_promote_knight_with_capture(FROM_SQ, TO_SQ),
        move_encode_promote_rook_with_capture(FROM_SQ, TO_SQ),
        move_encode_promote_bishop_with_capture(FROM_SQ, TO_SQ),
    };

    const enum piece prom_pces[] = {BLACK_QUEEN, BLACK_KNIGHT, BLACK_ROOK, BLACK_BISHOP};

    for (int i = 0; i < 4; i++) {
        const enum piece PROMOTION_PCE = prom_pces[i];

        struct position *pos = pos_create();
        pos_initialise(test_fen, pos);

        const uint64_t orig_pos_hash = pos_get_hash(pos);
        assert_true(orig_pos_hash != 0);

        const struct move mv = mv_list[i];

        uint64_t expected_hash = hash_piece_update(BLACK_PAWN, FROM_SQ, orig_pos_hash);
        expected_hash = hash_piece_update(WHITE_KNIGHT, TO_SQ, expected_hash);
        expected_hash = hash_piece_update(PROMOTION_PCE, TO_SQ, expected_hash);
        expected_hash = hash_side_update(expected_hash);

        pos_make_move(pos, mv);

        const uint64_t after_move_pos_hash = pos_get_hash(pos);
        assert_true(after_move_pos_hash != 0);
        assert_true(after_move_pos_hash == expected_hash);
        assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

        pos_destroy(pos);
    }
}

void test_position_hash_updated_white_move_quiet(void **state) {
    const char *test_fen = "8/8/3p4/2P4k/8/1B3K2/8/3N4 w - - 0 1\n";

    const enum square FROM_SQ = b3;
    const enum square TO_SQ = f7;

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    const struct move mv = move_encode_quiet(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update_move(WHITE_BISHOP, FROM_SQ, TO_SQ, orig_pos_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_white_move_capture(void **state) {
    const char *test_fen = "8/8/8/2Pp3k/8/1B3K2/8/3N4 w - - 0 1\n";

    const enum square FROM_SQ = b3;
    const enum square TO_SQ = d5;

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    const struct move mv = move_encode_capture(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update(BLACK_PAWN, TO_SQ, orig_pos_hash);
    expected_hash = hash_piece_update_move(WHITE_BISHOP, FROM_SQ, TO_SQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_black_move_quiet(void **state) {
    const char *test_fen = "8/5b2/3p4/2P4k/8/5K2/8/3N4 b - - 0 1\n";

    const enum square FROM_SQ = f7;
    const enum square TO_SQ = b3;

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    const struct move mv = move_encode_quiet(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update_move(BLACK_BISHOP, FROM_SQ, TO_SQ, orig_pos_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_black_move_capture(void **state) {
    const char *test_fen = "2r5/8/8/2Pp3k/8/1B3K2/8/3N4 b - - 0 1\n";

    const enum square FROM_SQ = c8;
    const enum square TO_SQ = c5;

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    const struct move mv = move_encode_capture(FROM_SQ, TO_SQ);

    uint64_t expected_hash = hash_piece_update(WHITE_PAWN, TO_SQ, orig_pos_hash);
    expected_hash = hash_piece_update_move(BLACK_ROOK, FROM_SQ, TO_SQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_white_king_castle(void **state) {
    const char *test_fen = "r3k2r/8/8/2Pp4/8/1B6/5N2/R3K2R w KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    const struct move mv = move_encode_castle_kingside_white();

    uint64_t expected_hash = hash_piece_update_move(WHITE_ROOK, h1, f1, orig_pos_hash);
    expected_hash = hash_piece_update_move(WHITE_KING, e1, g1, expected_hash);
    expected_hash = hash_castle_perm(CASTLE_PERM_WK, expected_hash);
    expected_hash = hash_castle_perm(CASTLE_PERM_WQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_white_queen_castle(void **state) {
    const char *test_fen = "r3k2r/8/8/2Pp4/8/1B6/5N2/R3K2R w KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    const struct move mv = move_encode_castle_queenside_white();

    uint64_t expected_hash = hash_piece_update_move(WHITE_ROOK, a1, d1, orig_pos_hash);
    expected_hash = hash_piece_update_move(WHITE_KING, e1, c1, expected_hash);
    expected_hash = hash_castle_perm(CASTLE_PERM_WK, expected_hash);
    expected_hash = hash_castle_perm(CASTLE_PERM_WQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_black_king_castle(void **state) {
    const char *test_fen = "r3k2r/8/8/2Pp4/8/1B6/5N2/R3K2R b KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    const struct move mv = move_encode_castle_kingside_black();

    uint64_t expected_hash = hash_piece_update_move(BLACK_ROOK, h8, f8, orig_pos_hash);
    expected_hash = hash_piece_update_move(BLACK_KING, e8, g8, expected_hash);
    expected_hash = hash_castle_perm(CASTLE_PERM_BK, expected_hash);
    expected_hash = hash_castle_perm(CASTLE_PERM_BQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

void test_position_hash_updated_black_queen_castle(void **state) {
    const char *test_fen = "r3k2r/8/8/2Pp4/8/1B6/5N2/R3K2R b KQkq - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(test_fen, pos);

    const uint64_t orig_pos_hash = pos_get_hash(pos);
    assert_true(orig_pos_hash != 0);

    const struct move mv = move_encode_castle_queenside_black();

    uint64_t expected_hash = hash_piece_update_move(BLACK_ROOK, a8, d8, orig_pos_hash);
    expected_hash = hash_piece_update_move(BLACK_KING, e8, c8, expected_hash);
    expected_hash = hash_castle_perm(CASTLE_PERM_BK, expected_hash);
    expected_hash = hash_castle_perm(CASTLE_PERM_BQ, expected_hash);
    expected_hash = hash_side_update(expected_hash);

    pos_make_move(pos, mv);

    const uint64_t after_move_pos_hash = pos_get_hash(pos);
    assert_true(after_move_pos_hash != 0);
    assert_true(after_move_pos_hash == expected_hash);
    assert_false(hash_compare(orig_pos_hash, after_move_pos_hash));

    pos_destroy(pos);
}

#pragma GCC diagnostic pop

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

#include "test_move.h"
#include "move.h"
#include "piece.h"
#include "square.h"
#include <cmocka.h>

void test_move_quiet_move_encode_decode(void **state) {
    for (enum square from_sq = a1; from_sq <= h8; from_sq++) {
        for (enum square to_sq = a1; to_sq <= h8; to_sq++) {
            if (from_sq == to_sq) {
                continue;
            }

            const struct move mv = move_encode_quiet(from_sq, to_sq);

            enum square decoded_from = move_decode_from_sq(mv);
            enum square decoded_to = move_decode_to_sq(mv);

            assert_true(from_sq == decoded_from);
            assert_true(to_sq == decoded_to);

            bool is_quiet = move_is_quiet(mv);
            assert_true(is_quiet);
        }
    }
}

void test_move_promoted_capture_move_encode_decode(void **state) {
    for (enum square from_sq = a1; from_sq <= h8; from_sq++) {
        for (enum square to_sq = a1; to_sq <= h8; to_sq++) {
            if (from_sq == to_sq) {
                continue;
            }

            struct move mv = move_encode_promote_queen_with_capture(from_sq, to_sq);

            enum square decoded_from = move_decode_from_sq(mv);
            enum square decoded_to = move_decode_to_sq(mv);

            assert_true(from_sq == decoded_from);
            assert_true(to_sq == decoded_to);

            bool is_capture = move_is_capture(mv);
            assert_true(is_capture);
        }
    }
}

void test_move_promoted_non_capture_move_encode_decode(void **state) {
    for (enum square from_sq = a1; from_sq <= h8; from_sq++) {
        for (enum square to_sq = a1; to_sq <= h8; to_sq++) {
            if (from_sq == to_sq) {
                continue;
            }

            struct move mv = move_encode_promote_queen(from_sq, to_sq);

            enum square decoded_from = move_decode_from_sq(mv);
            enum square decoded_to = move_decode_to_sq(mv);

            assert_true(from_sq == decoded_from);
            assert_true(to_sq == decoded_to);

            bool is_capture = move_is_capture(mv);
            assert_false(is_capture);
        }
    }
}

void test_move_en_passant_move_encode_decode(void **state) {
    for (enum square from_sq = a1; from_sq <= h8; from_sq++) {
        for (enum square to_sq = a1; to_sq <= h8; to_sq++) {
            if (from_sq == to_sq) {
                continue;
            }

            struct move mv = move_encode_enpassant(from_sq, to_sq);

            enum square decoded_from = move_decode_from_sq(mv);
            enum square decoded_to = move_decode_to_sq(mv);

            assert_true(from_sq == decoded_from);
            assert_true(to_sq == decoded_to);

            bool is_capture = move_is_capture(mv);
            assert_true(is_capture);

            bool is_enpassant = move_is_en_passant(mv);
            assert_true(is_enpassant);
        }
    }
}

void test_move_decode_promotion_piece_white(void **state) {
    const enum square from_sq = a7;
    const enum square to_sq = a8;
    struct move mv;
    enum piece pce;

    mv = move_encode_promote_knight_with_capture(from_sq, to_sq);
    pce = move_decode_promotion_piece(mv, WHITE);
    assert_true(pce == WHITE_KNIGHT);
    assert_true(pce_get_piece_role(pce) == KNIGHT);
    assert_true(pce_get_colour(pce) == WHITE);

    mv = move_encode_promote_bishop_with_capture(from_sq, to_sq);
    pce = move_decode_promotion_piece(mv, WHITE);
    assert_true(pce == WHITE_BISHOP);
    assert_true(pce_get_piece_role(pce) == BISHOP);
    assert_true(pce_get_colour(pce) == WHITE);

    mv = move_encode_promote_rook_with_capture(from_sq, to_sq);
    pce = move_decode_promotion_piece(mv, WHITE);
    assert_true(pce == WHITE_ROOK);
    assert_true(pce_get_piece_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == WHITE);

    mv = move_encode_promote_queen_with_capture(from_sq, to_sq);
    pce = move_decode_promotion_piece(mv, WHITE);
    assert_true(pce == WHITE_QUEEN);
    assert_true(pce_get_piece_role(pce) == QUEEN);
    assert_true(pce_get_colour(pce) == WHITE);
}

void test_move_decode_promotion_piece_black(void **state) {
    const enum square from_sq = a7;
    const enum square to_sq = a8;

    struct move mv = move_encode_promote_knight_with_capture(from_sq, to_sq);
    enum piece pce = move_decode_promotion_piece(mv, BLACK);
    assert_true(pce == BLACK_KNIGHT);
    assert_true(pce_get_piece_role(pce) == KNIGHT);
    assert_true(pce_get_colour(pce) == BLACK);

    mv = move_encode_promote_bishop_with_capture(from_sq, to_sq);
    pce = move_decode_promotion_piece(mv, BLACK);
    assert_true(pce == BLACK_BISHOP);
    assert_true(pce_get_piece_role(pce) == BISHOP);
    assert_true(pce_get_colour(pce) == BLACK);

    mv = move_encode_promote_rook_with_capture(from_sq, to_sq);
    pce = move_decode_promotion_piece(mv, BLACK);
    assert_true(pce == BLACK_ROOK);
    assert_true(pce_get_piece_role(pce) == ROOK);
    assert_true(pce_get_colour(pce) == BLACK);

    mv = move_encode_promote_queen_with_capture(from_sq, to_sq);
    pce = move_decode_promotion_piece(mv, BLACK);
    assert_true(pce == BLACK_QUEEN);
    assert_true(pce_get_piece_role(pce) == QUEEN);
    assert_true(pce_get_colour(pce) == BLACK);
}

void test_move_is_double_pawn_white(void **state) {
    for (enum square from_sq = a2; from_sq <= h2; from_sq++) {

        const enum square to_sq = (enum square)(from_sq + 16);
        struct move mv = move_encode_pawn_double_first(from_sq, to_sq);

        assert_true(move_is_double_pawn(mv));
    }
}

void test_move_is_double_pawn_black(void **state) {
    for (enum square from_sq = a7; from_sq <= h7; from_sq++) {

        const enum square to_sq = (enum square)(from_sq - 16);
        struct move mv = move_encode_pawn_double_first(from_sq, to_sq);

        assert_true(move_is_double_pawn(mv));
    }
}

void test_white_king_castle_encode_and_test(void **state) {
    struct move mv = move_encode_castle_kingside_white();

    enum square from_sq = move_decode_from_sq(mv);
    enum square to_sq = move_decode_to_sq(mv);
    assert_true(from_sq = e1);
    assert_true(to_sq = g1);
    assert_true(move_is_castle(mv));
    assert_true(move_is_king_castle(mv));
}

void test_white_queen_castle_encode_and_test(void **state) {
    struct move mv = move_encode_castle_queenside_white();

    enum square from_sq = move_decode_from_sq(mv);
    enum square to_sq = move_decode_to_sq(mv);
    assert_true(from_sq = e1);
    assert_true(to_sq = c1);
    assert_true(move_is_castle(mv));
    assert_true(move_is_queen_castle(mv));
}

void test_black_king_castle_encode_and_test(void **state) {
    struct move mv = move_encode_castle_kingside_black();

    enum square from_sq = move_decode_from_sq(mv);
    enum square to_sq = move_decode_to_sq(mv);
    assert_true(from_sq = e8);
    assert_true(to_sq = g8);
    assert_true(move_is_castle(mv));
    assert_true(move_is_king_castle(mv));
}

void test_black_queen_castle_encode_and_test(void **state) {
    struct move mv = move_encode_castle_queenside_black();

    enum square from_sq = move_decode_from_sq(mv);
    enum square to_sq = move_decode_to_sq(mv);
    assert_true(from_sq = e8);
    assert_true(to_sq = c8);
    assert_true(move_is_castle(mv));
    assert_true(move_is_queen_castle(mv));
}

void test_move_quiet_encode_decode(void **state) {
    const enum square from_sq = d4;
    const enum square to_sq = d5;

    const struct move mv = move_encode_quiet(from_sq, to_sq);

    assert_false(move_is_capture(mv));
    assert_false(move_is_castle(mv));
    assert_false(move_is_double_pawn(mv));
    assert_false(move_is_en_passant(mv));
    assert_false(move_is_king_castle(mv));
    assert_false(move_is_promotion(mv));
    assert_false(move_is_queen_castle(mv));
}

void test_move_capture_encode_decode(void **state) {
    const enum square from_sq = d4;
    const enum square to_sq = d5;

    const struct move mv = move_encode_capture(from_sq, to_sq);

    assert_false(move_is_quiet(mv));
    assert_false(move_is_castle(mv));
    assert_false(move_is_double_pawn(mv));
    assert_false(move_is_en_passant(mv));
    assert_false(move_is_king_castle(mv));
    assert_false(move_is_promotion(mv));
    assert_false(move_is_queen_castle(mv));
}

void test_move_white_king_castle_encode_decode(void **state) {
    const struct move mv = move_encode_castle_kingside_white();

    assert_false(move_is_quiet(mv));
    assert_false(move_is_double_pawn(mv));
    assert_false(move_is_en_passant(mv));
    assert_false(move_is_promotion(mv));
    assert_false(move_is_queen_castle(mv));
}

void test_move_white_queen_castle_encode_decode(void **state) {
    const struct move mv = move_encode_castle_queenside_white();

    assert_false(move_is_quiet(mv));
    assert_false(move_is_double_pawn(mv));
    assert_false(move_is_en_passant(mv));
    assert_false(move_is_promotion(mv));
    assert_false(move_is_king_castle(mv));
}

void test_move_black_king_castle_encode_decode(void **state) {
    const struct move mv = move_encode_castle_kingside_black();

    assert_false(move_is_quiet(mv));
    assert_false(move_is_double_pawn(mv));
    assert_false(move_is_en_passant(mv));
    assert_false(move_is_promotion(mv));
    assert_false(move_is_queen_castle(mv));
}

void test_move_black_queen_castle_encode_decode(void **state) {
    const struct move mv = move_encode_castle_queenside_black();

    assert_false(move_is_quiet(mv));
    assert_false(move_is_double_pawn(mv));
    assert_false(move_is_en_passant(mv));
    assert_false(move_is_promotion(mv));
    assert_false(move_is_king_castle(mv));
}

void test_move_double_pawn_move_encode_decode(void **state) {
    const enum square from_sq = d2;
    const enum square to_sq = d4;

    const struct move mv = move_encode_pawn_double_first(from_sq, to_sq);

    assert_false(move_is_capture(mv));
    assert_false(move_is_castle(mv));
    assert_false(move_is_en_passant(mv));
    assert_false(move_is_king_castle(mv));
    assert_false(move_is_promotion(mv));
    assert_false(move_is_queen_castle(mv));
}

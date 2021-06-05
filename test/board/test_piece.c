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

#include "test_piece.h"
#include "piece.h"
#include <cmocka.h>

static void label_piece_colour_validate(const char label, const enum piece_role pt, const enum colour col);

void test_piece_get_piece_from_label(void **state) {

    label_piece_colour_validate('P', PAWN, WHITE);
    label_piece_colour_validate('B', BISHOP, WHITE);
    label_piece_colour_validate('N', KNIGHT, WHITE);
    label_piece_colour_validate('R', ROOK, WHITE);
    label_piece_colour_validate('Q', QUEEN, WHITE);
    label_piece_colour_validate('K', KING, WHITE);

    label_piece_colour_validate('p', PAWN, BLACK);
    label_piece_colour_validate('b', BISHOP, BLACK);
    label_piece_colour_validate('n', KNIGHT, BLACK);
    label_piece_colour_validate('r', ROOK, BLACK);
    label_piece_colour_validate('q', QUEEN, BLACK);
    label_piece_colour_validate('k', KING, BLACK);
}

void test_piece_get_piece_label(void **state) {

    enum piece pce = WHITE_PAWN;
    assert_true(pce_get_label(pce) == 'P');

    pce = WHITE_BISHOP;
    assert_true(pce_get_label(pce) == 'B');

    pce = WHITE_KNIGHT;
    assert_true(pce_get_label(pce) == 'N');

    pce = WHITE_ROOK;
    assert_true(pce_get_label(pce) == 'R');

    pce = WHITE_QUEEN;
    assert_true(pce_get_label(pce) == 'Q');

    pce = WHITE_KING;
    assert_true(pce_get_label(pce) == 'K');

    pce = BLACK_PAWN;
    assert_true(pce_get_label(pce) == 'p');

    pce = BLACK_BISHOP;
    assert_true(pce_get_label(pce) == 'b');

    pce = BLACK_KNIGHT;
    assert_true(pce_get_label(pce) == 'n');

    pce = BLACK_ROOK;
    assert_true(pce_get_label(pce) == 'r');

    pce = BLACK_QUEEN;
    assert_true(pce_get_label(pce) == 'q');

    pce = BLACK_KING;
    assert_true(pce_get_label(pce) == 'k');
}

void test_piece_values(void **state) {
    assert_true(pce_get_value(PAWN) == 100);
    assert_true(pce_get_value(BISHOP) == 330);
    assert_true(pce_get_value(KNIGHT) == 320);
    assert_true(pce_get_value(ROOK) == 500);
    assert_true(pce_get_value(QUEEN) == 900);
    assert_true(pce_get_value(KING) == 20000);
}

void test_piece_get_colour_white_pieces(void **state) {

    enum piece pce = WHITE_PAWN;
    assert_true(pce_get_colour(pce) == WHITE);

    pce = WHITE_BISHOP;
    assert_true(pce_get_colour(pce) == WHITE);

    pce = WHITE_KNIGHT;
    assert_true(pce_get_colour(pce) == WHITE);

    pce = WHITE_ROOK;
    assert_true(pce_get_colour(pce) == WHITE);

    pce = WHITE_QUEEN;
    assert_true(pce_get_colour(pce) == WHITE);

    pce = WHITE_KING;
    assert_true(pce_get_colour(pce) == WHITE);
}

void test_piece_get_colour_black_pieces(void **state) {

    enum piece pce = BLACK_PAWN;
    assert_true(pce_get_colour(pce) == BLACK);

    pce = BLACK_BISHOP;
    assert_true(pce_get_colour(pce) == BLACK);

    pce = BLACK_KNIGHT;
    assert_true(pce_get_colour(pce) == BLACK);

    pce = BLACK_ROOK;
    assert_true(pce_get_colour(pce) == BLACK);

    pce = BLACK_QUEEN;
    assert_true(pce_get_colour(pce) == BLACK);

    pce = BLACK_KING;
    assert_true(pce_get_colour(pce) == BLACK);
}

void test_piece_get_array_idx(void **state) {
    assert_true(PCE_GET_ARRAY_INDEX(WHITE_PAWN) == 0);
    assert_true(PCE_GET_ARRAY_INDEX(WHITE_BISHOP) == 1);
    assert_true(PCE_GET_ARRAY_INDEX(WHITE_KNIGHT) == 2);
    assert_true(PCE_GET_ARRAY_INDEX(WHITE_ROOK) == 3);
    assert_true(PCE_GET_ARRAY_INDEX(WHITE_QUEEN) == 4);
    assert_true(PCE_GET_ARRAY_INDEX(WHITE_KING) == 5);
    assert_true(PCE_GET_ARRAY_INDEX(BLACK_PAWN) == 6);
    assert_true(PCE_GET_ARRAY_INDEX(BLACK_BISHOP) == 7);
    assert_true(PCE_GET_ARRAY_INDEX(BLACK_KNIGHT) == 8);
    assert_true(PCE_GET_ARRAY_INDEX(BLACK_ROOK) == 9);
    assert_true(PCE_GET_ARRAY_INDEX(BLACK_QUEEN) == 10);
    assert_true(PCE_GET_ARRAY_INDEX(BLACK_KING) == 11);
}

void test_piece_swap_side(void **state) {
    assert_true(pce_swap_side(WHITE) == BLACK);
    assert_true(pce_swap_side(BLACK) == WHITE);
}

void test_piece_is_white(void **state) {

    enum piece pce = WHITE_PAWN;
    assert_true(pce_is_white(pce));

    pce = WHITE_BISHOP;
    assert_true(pce_is_white(pce));

    pce = WHITE_KNIGHT;
    assert_true(pce_is_white(pce));

    pce = WHITE_ROOK;
    assert_true(pce_is_white(pce));

    pce = WHITE_QUEEN;
    assert_true(pce_is_white(pce));

    pce = WHITE_KING;
    assert_true(pce_is_white(pce));
}

void test_piece_role(void **state) {
    assert_true(pce_get_piece_role(WHITE_PAWN) == PAWN);
    assert_true(pce_get_piece_role(WHITE_BISHOP) == BISHOP);
    assert_true(pce_get_piece_role(WHITE_KNIGHT) == KNIGHT);
    assert_true(pce_get_piece_role(WHITE_ROOK) == ROOK);
    assert_true(pce_get_piece_role(WHITE_QUEEN) == QUEEN);
    assert_true(pce_get_piece_role(WHITE_KING) == KING);

    assert_true(pce_get_piece_role(BLACK_PAWN) == PAWN);
    assert_true(pce_get_piece_role(BLACK_BISHOP) == BISHOP);
    assert_true(pce_get_piece_role(BLACK_KNIGHT) == KNIGHT);
    assert_true(pce_get_piece_role(BLACK_ROOK) == ROOK);
    assert_true(pce_get_piece_role(BLACK_QUEEN) == QUEEN);
    assert_true(pce_get_piece_role(BLACK_KING) == KING);
}

void test_piece_is_black(void **state) {

    enum piece pce = BLACK_PAWN;
    assert_true(pce_is_black(pce));

    pce = BLACK_BISHOP;
    assert_true(pce_is_black(pce));

    pce = BLACK_KNIGHT;
    assert_true(pce_is_black(pce));

    pce = BLACK_ROOK;
    assert_true(pce_is_black(pce));

    pce = BLACK_QUEEN;
    assert_true(pce_is_black(pce));

    pce = BLACK_KING;
    assert_true(pce_is_black(pce));
}

void test_piece_is_king(void **state) {
    assert_true(pce_is_king(WHITE_KING));
    assert_true(pce_is_king(BLACK_KING));
}

static void label_piece_colour_validate(const char label, const enum piece_role pt, const enum colour col) {
    enum piece pce = pce_get_from_label(label);
    assert_true(pce_get_piece_role(pce) == pt);
    assert_true(pce_get_colour(pce) == col);
}

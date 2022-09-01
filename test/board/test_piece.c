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

static void label_piece_colour_validate(const char label, const enum piece pce, const enum colour col);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"

void test_piece_get_piece_from_label(void **state) {

    label_piece_colour_validate('P', WHITE_PAWN, WHITE);
    label_piece_colour_validate('B', WHITE_BISHOP, WHITE);
    label_piece_colour_validate('N', WHITE_KNIGHT, WHITE);
    label_piece_colour_validate('R', WHITE_ROOK, WHITE);
    label_piece_colour_validate('Q', WHITE_QUEEN, WHITE);
    label_piece_colour_validate('K', WHITE_KING, WHITE);

    label_piece_colour_validate('p', BLACK_PAWN, BLACK);
    label_piece_colour_validate('b', BLACK_BISHOP, BLACK);
    label_piece_colour_validate('n', BLACK_KNIGHT, BLACK);
    label_piece_colour_validate('r', BLACK_ROOK, BLACK);
    label_piece_colour_validate('q', BLACK_QUEEN, BLACK);
    label_piece_colour_validate('k', BLACK_KING, BLACK);
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

    assert_true(pce_get_value(BLACK_PAWN) == 100);
    assert_true(pce_get_value(BLACK_BISHOP) == 330);
    assert_true(pce_get_value(BLACK_KNIGHT) == 320);
    assert_true(pce_get_value(BLACK_ROOK) == 500);
    assert_true(pce_get_value(BLACK_QUEEN) == 900);
    assert_true(pce_get_value(BLACK_KING) == 20000);

    assert_true(pce_get_value(WHITE_PAWN) == 100);
    assert_true(pce_get_value(WHITE_BISHOP) == 330);
    assert_true(pce_get_value(WHITE_KNIGHT) == 320);
    assert_true(pce_get_value(WHITE_ROOK) == 500);
    assert_true(pce_get_value(WHITE_QUEEN) == 900);
    assert_true(pce_get_value(WHITE_KING) == 20000);
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

// WP,
// WB,
// WN,
// WR,
// WQ,
// WK,
// BP,
// BB,
// BN,
// BR,
// BQ,
// BK,

void test_piece_get_array_idx(void **state) {
    enum piece pce = WHITE_PAWN;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 0);
    pce = WHITE_BISHOP;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 1);
    pce = WHITE_KNIGHT;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 2);
    pce = WHITE_ROOK;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 3);
    pce = WHITE_QUEEN;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 4);
    pce = WHITE_KING;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 5);

    pce = BLACK_PAWN;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 6);
    pce = BLACK_BISHOP;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 7);
    pce = BLACK_KNIGHT;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 8);
    pce = BLACK_ROOK;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 9);
    pce = BLACK_QUEEN;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 10);
    pce = BLACK_KING;
    assert_true(PIECE_AS_ARRAY_OFFSET(pce) == 11);
}

void test_piece_swap_side(void **state) {
    assert_true(pce_swap_side(WHITE) == BLACK);
    assert_true(pce_swap_side(BLACK) == WHITE);
}

static void label_piece_colour_validate(const char label, const enum piece pce, const enum colour col) {
    const enum piece converted_pce = pce_get_from_label(label);
    const enum piece_role converted_role = pce_get_role(converted_pce);
    const enum colour converted_colour = pce_get_colour(converted_pce);

    assert_true(pce_get_role(pce) == converted_role);
    assert_true(pce_get_colour(pce) == converted_colour);
    assert_true(pce_get_label(pce) == label);
    assert_true(pce_get_label(converted_pce) == label);
}
#pragma GCC diagnostic pop

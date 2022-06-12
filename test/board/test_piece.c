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

static void label_piece_colour_validate(const char label, const struct piece *pce, const enum colour col);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"

void test_piece_get_piece_from_label(void **state) {

    label_piece_colour_validate('P', pce_get_white_pawn(), WHITE);
    label_piece_colour_validate('B', pce_get_white_bishop(), WHITE);
    label_piece_colour_validate('N', pce_get_white_knight(), WHITE);
    label_piece_colour_validate('R', pce_get_white_rook(), WHITE);
    label_piece_colour_validate('Q', pce_get_white_queen(), WHITE);
    label_piece_colour_validate('K', pce_get_white_king(), WHITE);

    label_piece_colour_validate('p', pce_get_black_pawn(), BLACK);
    label_piece_colour_validate('b', pce_get_black_bishop(), BLACK);
    label_piece_colour_validate('n', pce_get_black_knight(), BLACK);
    label_piece_colour_validate('r', pce_get_black_rook(), BLACK);
    label_piece_colour_validate('q', pce_get_black_queen(), BLACK);
    label_piece_colour_validate('k', pce_get_black_king(), BLACK);
}

void test_piece_get_piece_label(void **state) {

    struct piece *pce = pce_get_white_pawn();
    assert_true(pce_get_label(pce) == 'P');

    pce = pce_get_white_bishop();
    assert_true(pce_get_label(pce) == 'B');

    pce = pce_get_white_knight();
    assert_true(pce_get_label(pce) == 'N');

    pce = pce_get_white_rook();
    assert_true(pce_get_label(pce) == 'R');

    pce = pce_get_white_queen();
    assert_true(pce_get_label(pce) == 'Q');

    pce = pce_get_white_king();
    assert_true(pce_get_label(pce) == 'K');

    pce = pce_get_black_pawn();
    assert_true(pce_get_label(pce) == 'p');

    pce = pce_get_black_bishop();
    assert_true(pce_get_label(pce) == 'b');

    pce = pce_get_black_knight();
    assert_true(pce_get_label(pce) == 'n');

    pce = pce_get_black_rook();
    assert_true(pce_get_label(pce) == 'r');

    pce = pce_get_black_queen();
    assert_true(pce_get_label(pce) == 'q');

    pce = pce_get_black_king();
    assert_true(pce_get_label(pce) == 'k');
}

void test_piece_values(void **state) {

    assert_true(pce_get_value(pce_get_white_pawn()) == 100);
    assert_true(pce_get_value(pce_get_white_bishop()) == 330);
    assert_true(pce_get_value(pce_get_white_knight()) == 320);
    assert_true(pce_get_value(pce_get_white_rook()) == 500);
    assert_true(pce_get_value(pce_get_white_queen()) == 900);
    assert_true(pce_get_value(pce_get_white_king()) == 20000);

    assert_true(pce_get_value(pce_get_black_pawn()) == 100);
    assert_true(pce_get_value(pce_get_black_bishop()) == 330);
    assert_true(pce_get_value(pce_get_black_knight()) == 320);
    assert_true(pce_get_value(pce_get_black_rook()) == 500);
    assert_true(pce_get_value(pce_get_black_queen()) == 900);
    assert_true(pce_get_value(pce_get_black_king()) == 20000);
}

void test_piece_get_colour_white_pieces(void **state) {

    struct piece *pce = pce_get_white_pawn();
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_get_white_bishop();
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_get_white_knight();
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_get_white_rook();
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_get_white_queen();
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_get_white_king();
    assert_true(pce_get_colour(pce) == WHITE);
}

void test_piece_get_colour_black_pieces(void **state) {

    struct piece *pce = pce_get_black_pawn();
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_get_black_bishop();
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_get_black_knight();
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_get_black_rook();
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_get_black_queen();
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_get_black_king();
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
    const struct piece *pce = pce_get_white_pawn();
    assert_true(pce_get_array_offset(pce) == 0);
    pce = pce_get_white_bishop();
    assert_true(pce_get_array_offset(pce) == 1);
    pce = pce_get_white_knight();
    assert_true(pce_get_array_offset(pce) == 2);
    pce = pce_get_white_rook();
    assert_true(pce_get_array_offset(pce) == 3);
    pce = pce_get_white_queen();
    assert_true(pce_get_array_offset(pce) == 4);
    pce = pce_get_white_king();
    assert_true(pce_get_array_offset(pce) == 5);

    pce = pce_get_black_pawn();
    assert_true(pce_get_array_offset(pce) == 6);
    pce = pce_get_black_bishop();
    assert_true(pce_get_array_offset(pce) == 7);
    pce = pce_get_black_knight();
    assert_true(pce_get_array_offset(pce) == 8);
    pce = pce_get_black_rook();
    assert_true(pce_get_array_offset(pce) == 9);
    pce = pce_get_black_queen();
    assert_true(pce_get_array_offset(pce) == 10);
    pce = pce_get_black_king();
    assert_true(pce_get_array_offset(pce) == 11);
}

void test_piece_swap_side(void **state) {
    assert_true(pce_swap_side(WHITE) == BLACK);
    assert_true(pce_swap_side(BLACK) == WHITE);
}

static void label_piece_colour_validate(const char label, const struct piece *pce, const enum colour col) {
    const struct piece *converted_pce = pce_get_from_label(label);
    const enum piece_role converted_role = pce_get_role(converted_pce);
    const enum colour converted_colour = pce_get_colour(converted_pce);

    assert_true(pce_get_role(pce) == converted_role);
    assert_true(pce_get_colour(pce) == converted_colour);
    assert_true(pce_get_label(pce) == label);
    assert_true(pce_get_label(converted_pce) == label);
}
#pragma GCC diagnostic pop

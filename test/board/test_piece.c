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

static void piece_create_validate(const enum piece_type pt,
                                  const enum colour col);
static void label_piece_colour_validate(const char label,
                                        const enum piece_type pt,
                                        const enum colour col);

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

    struct piece pce = pce_create(PAWN, WHITE);
    assert_true(pce_get_label(pce) == 'P');

    pce = pce_create(BISHOP, WHITE);
    assert_true(pce_get_label(pce) == 'B');

    pce = pce_create(KNIGHT, WHITE);
    assert_true(pce_get_label(pce) == 'N');

    pce = pce_create(ROOK, WHITE);
    assert_true(pce_get_label(pce) == 'R');

    pce = pce_create(QUEEN, WHITE);
    assert_true(pce_get_label(pce) == 'Q');

    pce = pce_create(KING, WHITE);
    assert_true(pce_get_label(pce) == 'K');

    pce = pce_create(PAWN, BLACK);
    assert_true(pce_get_label(pce) == 'p');

    pce = pce_create(BISHOP, BLACK);
    assert_true(pce_get_label(pce) == 'b');

    pce = pce_create(KNIGHT, BLACK);
    assert_true(pce_get_label(pce) == 'n');

    pce = pce_create(ROOK, BLACK);
    assert_true(pce_get_label(pce) == 'r');

    pce = pce_create(QUEEN, BLACK);
    assert_true(pce_get_label(pce) == 'q');

    pce = pce_create(KING, BLACK);
    assert_true(pce_get_label(pce) == 'k');
}

void test_piece_values(void **state) {
    assert_true(pce_get_value(PAWN) == 100);
    assert_true(pce_get_value(BISHOP) == 325);
    assert_true(pce_get_value(KNIGHT) == 325);
    assert_true(pce_get_value(ROOK) == 500);
    assert_true(pce_get_value(QUEEN) == 1000);
    assert_true(pce_get_value(KING) == 50000);
}

void test_piece_get_colour_white_pieces(void **state) {

    struct piece pce = pce_create(PAWN, WHITE);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_create(BISHOP, WHITE);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_create(KNIGHT, WHITE);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_create(ROOK, WHITE);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_create(QUEEN, WHITE);
    assert_true(pce_get_colour(pce) == WHITE);

    pce = pce_create(KING, WHITE);
    assert_true(pce_get_colour(pce) == WHITE);
}

void test_piece_get_colour_black_pieces(void **state) {

    struct piece pce = pce_create(PAWN, BLACK);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_create(BISHOP, BLACK);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_create(KNIGHT, BLACK);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_create(ROOK, BLACK);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_create(QUEEN, BLACK);
    assert_true(pce_get_colour(pce) == BLACK);

    pce = pce_create(KING, BLACK);
    assert_true(pce_get_colour(pce) == BLACK);
}

void test_piece_get_array_idx(void **state) {
    assert_true(pce_get_array_idx(PAWN) == 0);
    assert_true(pce_get_array_idx(BISHOP) == 1);
    assert_true(pce_get_array_idx(KNIGHT) == 2);
    assert_true(pce_get_array_idx(ROOK) == 3);
    assert_true(pce_get_array_idx(QUEEN) == 4);
    assert_true(pce_get_array_idx(KING) == 5);
}

void test_piece_swap_side(void **state) {
    assert_true(pce_swap_side(WHITE) == BLACK);
    assert_true(pce_swap_side(BLACK) == WHITE);
}

void test_piece_is_white(void **state) {

    struct piece pce = pce_create(PAWN, WHITE);
    assert_true(pce_is_white(pce));

    pce = pce_create(BISHOP, WHITE);
    assert_true(pce_is_white(pce));

    pce = pce_create(KNIGHT, WHITE);
    assert_true(pce_is_white(pce));

    pce = pce_create(ROOK, WHITE);
    assert_true(pce_is_white(pce));

    pce = pce_create(QUEEN, WHITE);
    assert_true(pce_is_white(pce));

    pce = pce_create(KING, WHITE);
    assert_true(pce_is_white(pce));
}

void test_piece_is_black(void **state) {

    struct piece pce = pce_create(PAWN, BLACK);
    assert_true(pce_is_black(pce));

    pce = pce_create(BISHOP, BLACK);
    assert_true(pce_is_black(pce));

    pce = pce_create(KNIGHT, BLACK);
    assert_true(pce_is_black(pce));

    pce = pce_create(ROOK, BLACK);
    assert_true(pce_is_black(pce));

    pce = pce_create(QUEEN, BLACK);
    assert_true(pce_is_black(pce));

    pce = pce_create(KING, BLACK);
    assert_true(pce_is_black(pce));
}

void test_piece_create(void **state) {

    piece_create_validate(PAWN, WHITE);
    piece_create_validate(BISHOP, WHITE);
    piece_create_validate(KNIGHT, WHITE);
    piece_create_validate(ROOK, WHITE);
    piece_create_validate(QUEEN, WHITE);
    piece_create_validate(KING, WHITE);

    piece_create_validate(PAWN, BLACK);
    piece_create_validate(BISHOP, BLACK);
    piece_create_validate(KNIGHT, BLACK);
    piece_create_validate(ROOK, BLACK);
    piece_create_validate(QUEEN, BLACK);
    piece_create_validate(KING, BLACK);
}

static void piece_create_validate(const enum piece_type pt,
                                  const enum colour col) {
    struct piece pce = pce_create(pt, col);
    enum colour found_col = pce_get_colour(pce);
    assert_true(found_col == col);
    enum piece_type found_pt = pce_get_piece_type(pce);
    assert_true(found_pt == pt);
}

static void label_piece_colour_validate(const char label,
                                        const enum piece_type pt,
                                        const enum colour col) {
    struct piece pce = pce_get_from_label(label);
    assert_true(pce_get_piece_type(pce) == pt);
    assert_true(pce_get_colour(pce) == col);
}

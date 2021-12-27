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

static void label_piece_colour_validate(const char label, const enum piece pt, const enum colour col);

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
    piece_init();

    assert_true(pce_get_value(WHITE_PAWN) == 100);
    assert_true(pce_get_value(WHITE_BISHOP) == 330);
    assert_true(pce_get_value(WHITE_KNIGHT) == 320);
    assert_true(pce_get_value(WHITE_ROOK) == 500);
    assert_true(pce_get_value(WHITE_QUEEN) == 900);
    assert_true(pce_get_value(WHITE_KING) == 20000);

    assert_true(pce_get_value(BLACK_PAWN) == 100);
    assert_true(pce_get_value(BLACK_BISHOP) == 330);
    assert_true(pce_get_value(BLACK_KNIGHT) == 320);
    assert_true(pce_get_value(BLACK_ROOK) == 500);
    assert_true(pce_get_value(BLACK_QUEEN) == 900);
    assert_true(pce_get_value(BLACK_KING) == 20000);
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
    assert_true(PIECE_AS_ARRAY_OFFSET(WHITE_PAWN) == 0);
    assert_true(PIECE_AS_ARRAY_OFFSET(BLACK_PAWN) == 1);

    assert_true(PIECE_AS_ARRAY_OFFSET(WHITE_BISHOP) == 2);
    assert_true(PIECE_AS_ARRAY_OFFSET(BLACK_BISHOP) == 3);

    assert_true(PIECE_AS_ARRAY_OFFSET(WHITE_KNIGHT) == 4);
    assert_true(PIECE_AS_ARRAY_OFFSET(BLACK_KNIGHT) == 5);

    assert_true(PIECE_AS_ARRAY_OFFSET(WHITE_ROOK) == 6);
    assert_true(PIECE_AS_ARRAY_OFFSET(BLACK_ROOK) == 7);

    assert_true(PIECE_AS_ARRAY_OFFSET(WHITE_QUEEN) == 8);
    assert_true(PIECE_AS_ARRAY_OFFSET(BLACK_QUEEN) == 9);

    assert_true(PIECE_AS_ARRAY_OFFSET(WHITE_KING) == 10);
    assert_true(PIECE_AS_ARRAY_OFFSET(BLACK_KING) == 11);
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

void test_piece_is_pawn(void **state) {
    assert_true(pce_is_pawn(WHITE_PAWN));
    assert_true(pce_is_pawn(BLACK_PAWN));
}

void test_piece_is_bishop(void **state) {
    assert_true(pce_is_bishop(WHITE_BISHOP));
    assert_true(pce_is_bishop(BLACK_BISHOP));
}

void test_piece_is_knight(void **state) {
    assert_true(pce_is_knight(WHITE_KNIGHT));
    assert_true(pce_is_knight(BLACK_KNIGHT));
}

void test_piece_is_rook(void **state) {
    assert_true(pce_is_rook(WHITE_ROOK));
    assert_true(pce_is_rook(BLACK_ROOK));
}
void test_piece_is_queen(void **state) {
    assert_true(pce_is_queen(WHITE_QUEEN));
    assert_true(pce_is_queen(BLACK_QUEEN));
}

static void label_piece_colour_validate(const char label, const enum piece pce, const enum colour col) {
    enum piece converted_pce = pce_get_from_label(label);
    assert_true(converted_pce == pce);
    assert_true(pce_get_colour(pce) == col);
}

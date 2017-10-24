#include <stdbool.h>
#include <stdint.h>
#include "seatest.h"
#include "piece.h"
#include "test_piece.h"


static void test_is_white(void);
static void test_is_black(void);
static void test_get_colour_white_pieces(void);
static void test_get_colour_black_pieces(void);
static void test_swap_side(void);
static void test_get_piece_label(void);
static void test_piece_values(void);
static void test_get_piece_from_label(void);

void test_fixture_piece(void)
{
	test_fixture_start(); // starts a fixture

	run_test(test_is_white);
	run_test(test_is_black);
	run_test(test_get_colour_white_pieces);
	run_test(test_get_colour_black_pieces);
	run_test(test_swap_side);
	run_test(test_get_piece_label);
	run_test(test_piece_values);
	run_test(test_get_piece_from_label);

	test_fixture_end(); // ends a fixture
}

static void test_get_piece_from_label(void)
{
	assert_true(pce_get_from_label('P') == WPAWN);
	assert_true(pce_get_from_label('B') == WBISHOP);
	assert_true(pce_get_from_label('N') == WKNIGHT);
	assert_true(pce_get_from_label('R') == WROOK);
	assert_true(pce_get_from_label('Q') == WQUEEN);
	assert_true(pce_get_from_label('K') == WKING);

	assert_true(pce_get_from_label('p') == BPAWN);
	assert_true(pce_get_from_label('b') == BBISHOP);
	assert_true(pce_get_from_label('n') == BKNIGHT);
	assert_true(pce_get_from_label('r') == BROOK);
	assert_true(pce_get_from_label('q') == BQUEEN);
	assert_true(pce_get_from_label('k') == BKING);
}

static void test_get_piece_label(void)
{
	assert_true(pce_get_label(WPAWN) == 'P');
	assert_true(pce_get_label(BPAWN) == 'p');
	assert_true(pce_get_label(WBISHOP) == 'B');
	assert_true(pce_get_label(BBISHOP) == 'b');
	assert_true(pce_get_label(WKNIGHT) == 'N');
	assert_true(pce_get_label(BKNIGHT) == 'n');
	assert_true(pce_get_label(WROOK) == 'R');
	assert_true(pce_get_label(BROOK) == 'r');
	assert_true(pce_get_label(WQUEEN) == 'Q');
	assert_true(pce_get_label(BQUEEN) == 'q');
	assert_true(pce_get_label(WKING) == 'K');
	assert_true(pce_get_label(BKING) == 'k');
}

static void test_piece_values(void)
{
	assert_true(pce_get_value(WPAWN) == 100);
	assert_true(pce_get_value(BPAWN) == 100);
	assert_true(pce_get_value(WPAWN) == pce_get_value(BPAWN));

	assert_true(pce_get_value(WBISHOP) == 325);
	assert_true(pce_get_value(BBISHOP) == 325);
	assert_true(pce_get_value(WBISHOP) == pce_get_value(BBISHOP));

	assert_true(pce_get_value(WKNIGHT) == 325);
	assert_true(pce_get_value(BKNIGHT) == 325);
	assert_true(pce_get_value(WKNIGHT) == pce_get_value(BKNIGHT));

	assert_true(pce_get_value(WROOK) == 500);
	assert_true(pce_get_value(BROOK) == 500);
	assert_true(pce_get_value(WROOK) == pce_get_value(BROOK));

	assert_true(pce_get_value(WQUEEN) == 1000);
	assert_true(pce_get_value(BQUEEN) == 1000);
	assert_true(pce_get_value(WQUEEN) == pce_get_value(BQUEEN));

	assert_true(pce_get_value(WKING) == 50000);
	assert_true(pce_get_value(BKING) == 50000);
	assert_true(pce_get_value(WKING) == pce_get_value(BKING));
}

static void test_get_colour_white_pieces(void)
{
	assert_true(pce_get_colour(WPAWN) == WHITE);
	assert_true(pce_get_colour(WBISHOP) == WHITE);
	assert_true(pce_get_colour(WKNIGHT) == WHITE);
	assert_true(pce_get_colour(WROOK) == WHITE);
	assert_true(pce_get_colour(WQUEEN) == WHITE);
	assert_true(pce_get_colour(WKING) == WHITE);
}

static void test_get_colour_black_pieces(void)
{
	assert_true(pce_get_colour(BPAWN) == BLACK);
	assert_true(pce_get_colour(BBISHOP) == BLACK);
	assert_true(pce_get_colour(BKNIGHT) == BLACK);
	assert_true(pce_get_colour(BROOK) == BLACK);
	assert_true(pce_get_colour(BQUEEN) == BLACK);
	assert_true(pce_get_colour(BKING) == BLACK);
}

static void test_swap_side(void)
{
	assert_true(swap_side(WHITE) == BLACK);
	assert_true(swap_side(BLACK) == WHITE);
}

static void test_is_white(void)
{
	assert_true(pce_is_white(WPAWN));
	assert_true(pce_is_white(WBISHOP));
	assert_true(pce_is_white(WKNIGHT));
	assert_true(pce_is_white(WROOK));
	assert_true(pce_is_white(WQUEEN));
	assert_true(pce_is_white(WKING));
}

static void test_is_black(void)
{
	assert_true(pce_is_black(BPAWN));
	assert_true(pce_is_black(BBISHOP));
	assert_true(pce_is_black(BKNIGHT));
	assert_true(pce_is_black(BROOK));
	assert_true(pce_is_black(BQUEEN));
	assert_true(pce_is_black(BKING));
}



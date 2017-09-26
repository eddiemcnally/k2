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


void test_fixture_piece(void)
{
	test_fixture_start();               // starts a fixture

	run_test(test_is_white);
	run_test(test_is_black);
	run_test(test_get_colour_white_pieces);
	run_test(test_get_colour_black_pieces);
	run_test(test_swap_side);
	run_test(test_get_piece_label);
	run_test(test_piece_values);

	test_fixture_end();                 // ends a fixture
}


static void test_get_piece_label(void)
{
	assert_true(get_label(WPAWN) 	== 'P');
	assert_true(get_label(BPAWN) 	== 'p');
	assert_true(get_label(WBISHOP) 	== 'B');
	assert_true(get_label(BBISHOP) 	== 'b');
	assert_true(get_label(WKNIGHT) 	== 'N');
	assert_true(get_label(BKNIGHT) 	== 'n');
	assert_true(get_label(WROOK) 	== 'R');
	assert_true(get_label(BROOK) 	== 'r');
	assert_true(get_label(WQUEEN) 	== 'Q');
	assert_true(get_label(BQUEEN) 	== 'q');
	assert_true(get_label(WKING) 	== 'K');
	assert_true(get_label(BKING) 	== 'k');
}

static void test_piece_values(void)
{
	assert_true(get_piece_value(WPAWN) 		== 100);
	assert_true(get_piece_value(BPAWN) 		== 100);
	assert_true(get_piece_value(WPAWN) 		== get_piece_value(BPAWN));

	assert_true(get_piece_value(WBISHOP) 	== 325);
	assert_true(get_piece_value(BBISHOP) 	== 325);
	assert_true(get_piece_value(WBISHOP) 	== get_piece_value(BBISHOP));

	assert_true(get_piece_value(WKNIGHT) 	== 325);
	assert_true(get_piece_value(BKNIGHT) 	== 325);
	assert_true(get_piece_value(WKNIGHT) 	== get_piece_value(BKNIGHT));

	assert_true(get_piece_value(WROOK) 		== 550);
	assert_true(get_piece_value(BROOK) 		== 550);
	assert_true(get_piece_value(WROOK) 		== get_piece_value(BROOK));

	assert_true(get_piece_value(WQUEEN) 	== 1000);
	assert_true(get_piece_value(BQUEEN) 	== 1000);
	assert_true(get_piece_value(WQUEEN) 	== get_piece_value(BQUEEN));

	assert_true(get_piece_value(WKING) 		== 50000);
	assert_true(get_piece_value(BKING) 		== 50000);
	assert_true(get_piece_value(WKING) 		== get_piece_value(BKING));
}

static void test_get_colour_white_pieces(void)
{
	assert_true(get_colour(WPAWN) 	== WHITE);
	assert_true(get_colour(WBISHOP) == WHITE);
	assert_true(get_colour(WKNIGHT) == WHITE);
	assert_true(get_colour(WROOK) 	== WHITE);
	assert_true(get_colour(WQUEEN) 	== WHITE);
	assert_true(get_colour(WKING) 	== WHITE);
}


static void test_get_colour_black_pieces(void)
{
	assert_true(get_colour(BPAWN) 	== BLACK);
	assert_true(get_colour(BBISHOP) == BLACK);
	assert_true(get_colour(BKNIGHT) == BLACK);
	assert_true(get_colour(BROOK) 	== BLACK);
	assert_true(get_colour(BQUEEN) 	== BLACK);
	assert_true(get_colour(BKING) 	== BLACK);
}


static void test_swap_side(void)
{
	assert_true(swap_side(WHITE) == BLACK);
	assert_true(swap_side(BLACK) == WHITE);
}


static void test_is_white(void)
{
	assert_true(is_white(WPAWN));
	assert_true(is_white(WBISHOP));
	assert_true(is_white(WKNIGHT));
	assert_true(is_white(WROOK));
	assert_true(is_white(WQUEEN));
	assert_true(is_white(WKING));
}

static void test_is_black(void)
{
	assert_true(is_black(BPAWN));
	assert_true(is_black(BBISHOP));
	assert_true(is_black(BKNIGHT));
	assert_true(is_black(BROOK));
	assert_true(is_black(BQUEEN));
	assert_true(is_black(BKING));
}



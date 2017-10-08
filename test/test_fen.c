#include <stdbool.h>
#include <stdint.h>
#include "seatest.h"
#include "piece.h"
#include "position.h"
#include "square.h"
#include "fen.h"
#include "test_fen.h"


static void test_fen_pieces_init_position(void);
static void test_fen_pieces_random_position(void);
static void test_fen_side_to_move(void);
static void test_fen_castle_permissions(void);
static void test_fen_half_move_count(void);
static void test_fen_full_move_count(void);


void test_fixture_fen(void)
{
	test_fixture_start();               // starts a fixture

	run_test(test_fen_pieces_init_position);
	run_test(test_fen_pieces_random_position);
	run_test(test_fen_side_to_move);
	run_test(test_fen_castle_permissions);
	run_test(test_fen_half_move_count);
	run_test(test_fen_full_move_count);

	test_fixture_end();                 // ends a fixture
}





static void test_fen_pieces_init_position(void)
{

	struct parsed_fen* brd = parse_fen(INITIAL_FEN);

	bool found = false;
	enum piece pce;

	found = try_get_piece_on_sq(brd, a1, &pce);
	assert_true(found);
	assert_true(pce == WROOK);

	found = try_get_piece_on_sq(brd, b1, &pce);
	assert_true(found);
	assert_true(pce == WKNIGHT);



}

static void test_fen_pieces_random_position(void)
{

}

static void test_fen_side_to_move(void)
{

}

static void test_fen_castle_permissions(void)
{

}

static void test_fen_half_move_count(void)
{

}

static void test_fen_full_move_count(void)
{

}

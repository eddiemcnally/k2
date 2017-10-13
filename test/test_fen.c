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

	found = try_get_piece_on_sq(brd, c1, &pce);
	assert_true(found);
	assert_true(pce == WBISHOP);

	found = try_get_piece_on_sq(brd, d1, &pce);
	assert_true(found);
	assert_true(pce == WQUEEN);

	found = try_get_piece_on_sq(brd, e1, &pce);
	assert_true(found);
	assert_true(pce == WKING);

	found = try_get_piece_on_sq(brd, f1, &pce);
	assert_true(found);
	assert_true(pce == WBISHOP);

	found = try_get_piece_on_sq(brd, g1, &pce);
	assert_true(found);
	assert_true(pce == WKNIGHT);

	found = try_get_piece_on_sq(brd, h1, &pce);
	assert_true(found);
	assert_true(pce == WROOK);

	enum rank r = RANK_2;
	for (int f = FILE_A; f <= FILE_H; f++) {
		enum square sq = get_square(r, (enum file)f);

		found = try_get_piece_on_sq(brd, sq, &pce);
		assert_true(found);
		assert_true(pce == WPAWN);
	}


	found = try_get_piece_on_sq(brd, a8, &pce);
	assert_true(found);
	assert_true(pce == BROOK);

	found = try_get_piece_on_sq(brd, b8, &pce);
	assert_true(found);
	assert_true(pce == BKNIGHT);

	found = try_get_piece_on_sq(brd, c8, &pce);
	assert_true(found);
	assert_true(pce == BBISHOP);

	found = try_get_piece_on_sq(brd, d8, &pce);
	assert_true(found);
	assert_true(pce == BQUEEN);

	found = try_get_piece_on_sq(brd, e8, &pce);
	assert_true(found);
	assert_true(pce == BKING);

	found = try_get_piece_on_sq(brd, f8, &pce);
	assert_true(found);
	assert_true(pce == BBISHOP);

	found = try_get_piece_on_sq(brd, g8, &pce);
	assert_true(found);
	assert_true(pce == BKNIGHT);

	found = try_get_piece_on_sq(brd, h8, &pce);
	assert_true(found);
	assert_true(pce == BROOK);

	r = RANK_7;
	for (int f = FILE_A; f <= FILE_H; f++) {
		enum square sq = get_square(r, (enum file)f);

		found = try_get_piece_on_sq(brd, sq, &pce);
		assert_true(found);
		assert_true(pce == BPAWN);
	}


	// all other squares are empty
	for (r = RANK_3; r <= RANK_6; r++) {
		for (int f = FILE_A; f <= FILE_H; f++) {
			enum square sq = get_square((enum rank)r, (enum file)f);

			found = try_get_piece_on_sq(brd, sq, &pce);
			assert_false(found);
		}
	}
}


static void test_fen_pieces_random_position(void)
{
	char *random_fen_1 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 w Qkq -\n";


	struct parsed_fen* brd = parse_fen(random_fen_1);

	bool found = false;
	enum piece pce;

	// RANK 1
	found = try_get_piece_on_sq(brd, a1, &pce);
	assert_true(found);
	assert_true(pce == WROOK);

	enum rank r = RANK_1;
	for (int f = FILE_B; f <= FILE_E; f++) {
		enum square sq = get_square(r, (enum file)f);

		found = try_get_piece_on_sq(brd, sq, &pce);
		assert_false(found);
	}

	found = try_get_piece_on_sq(brd, f1, &pce);
	assert_true(found);
	assert_true(pce == WROOK);

	found = try_get_piece_on_sq(brd, g1, &pce);
	assert_true(found);
	assert_true(pce == WKING);

	found = try_get_piece_on_sq(brd, h1, &pce);
	assert_false(found);


	// RANK 2
	found = try_get_piece_on_sq(brd, a2, &pce);
	assert_false(found);

	found = try_get_piece_on_sq(brd, b2, &pce);
	assert_true(found);
	assert_true(pce == WBISHOP);

	found = try_get_piece_on_sq(brd, c2, &pce);
	assert_false(found);

	found = try_get_piece_on_sq(brd, d2, &pce);
	assert_true(found);
	assert_true(pce == WPAWN);

	found = try_get_piece_on_sq(brd, e2, &pce);
	assert_true(found);
	assert_true(pce == WQUEEN);

	found = try_get_piece_on_sq(brd, f2, &pce);
	assert_true(found);
	assert_true(pce == WPAWN);

	found = try_get_piece_on_sq(brd, g2, &pce);
	assert_false(found);

	found = try_get_piece_on_sq(brd, h2, &pce);
	assert_true(found);
	assert_true(pce == WPAWN);








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

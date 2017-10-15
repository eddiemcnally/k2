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
static void test_fen_castle_permissions_initial_fen(void);
static void test_fen_half_move_count(void);
static void test_fen_full_move_count(void);

static const char *RANDOM_FEN_1 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 w Qkq - 1 4\n";
static const char *RANDOM_FEN_2 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b KQkq - 0 3\n";
static const char *RANDOM_FEN_3 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b Kq - 1 2\n";
static const char *RANDOM_FEN_4 = "r6r/p1pkqp1p/5n2/np1pp1p1/1bP1P3/PPNB1NPb/1B1PQP1P/R4RK1 b - - 0 3\n";


struct sq_pce {
	enum piece piece;
	bool has_piece;
};


void test_fixture_fen(void)
{
	test_fixture_start();               // starts a fixture

	run_test(test_fen_pieces_init_position);
	run_test(test_fen_pieces_random_position);
	run_test(test_fen_side_to_move);
	run_test(test_fen_castle_permissions_initial_fen);
	run_test(test_fen_half_move_count);
	run_test(test_fen_full_move_count);

	test_fixture_end();                 // ends a fixture
}





static void test_fen_pieces_init_position(void)
{
	struct parsed_fen* brd = parse_fen(INITIAL_FEN);

	// setup test data for comparison
	struct sq_pce data[NUM_SQUARES];
	for (int i = a1; i < h8; i++) {
		data[a1].has_piece = false;
	}

	data[a1].has_piece = true;
	data[a1].piece = WROOK;
	data[b1].has_piece = true;
	data[b1].piece = WKNIGHT;
	data[c1].has_piece = true;
	data[c1].piece = WBISHOP;
	data[d1].has_piece = true;
	data[d1].piece = WQUEEN;
	data[e1].has_piece = true;
	data[e1].piece = WKING;
	data[f1].has_piece = true;
	data[f1].piece = WBISHOP;
	data[g1].has_piece = true;
	data[g1].piece = WKNIGHT;
	data[h1].has_piece = true;
	data[h1].piece = WROOK;
	for (int i = a2; i <= h2; i++) {
		data[i].has_piece = true;
		data[i].piece = WPAWN;
	}

	data[a8].has_piece = true;
	data[a8].piece = BROOK;
	data[b8].has_piece = true;
	data[b8].piece = BKNIGHT;
	data[c8].has_piece = true;
	data[c8].piece = BBISHOP;
	data[d8].has_piece = true;
	data[d8].piece = BQUEEN;
	data[e8].has_piece = true;
	data[e8].piece = BKING;
	data[f8].has_piece = true;
	data[f8].piece = BBISHOP;
	data[g8].has_piece = true;
	data[g8].piece = BKNIGHT;
	data[h8].has_piece = true;
	data[h8].piece = BROOK;
	for (int i = a7; i <= h7; i++) {
		data[i].has_piece = true;
		data[i].piece = BPAWN;
	}

	bool found = false;
	enum piece pce;
	for ( int s = a1; s <= h8; s++) {
		struct sq_pce d = data[s];

		bool b = try_get_piece_on_sq(brd, s, &pce);

		if (b == true) {
			assert_true(pce == d.piece);
		}
	}

	// all other squares are empty
	for (int r = RANK_3; r <= RANK_6; r++) {
		for (int f = FILE_A; f <= FILE_H; f++) {
			enum square sq = get_square((enum rank)r, (enum file)f);

			found = try_get_piece_on_sq(brd, sq, &pce);
			assert_false(found);
		}
	}
}


static void test_fen_pieces_random_position(void)
{
	struct parsed_fen* brd = parse_fen(RANDOM_FEN_1);

	// setup test data for comparison
	struct sq_pce data[NUM_SQUARES];
	for (int i = a1; i < h8; i++) {
		data[a1].has_piece = false;
	}

	// RANK 1
	data[a1].has_piece = true;
	data[a1].piece = WROOK;
	data[f1].has_piece = true;
	data[f1].piece = WROOK;
	data[g1].has_piece = true;
	data[g1].piece = WKING;

	// RANK 2
	data[b2].has_piece = true;
	data[b2].piece = WBISHOP;
	data[d2].has_piece = true;
	data[d2].piece = WPAWN;
	data[e2].has_piece = true;
	data[e2].piece = WQUEEN;
	data[f2].has_piece = true;
	data[f2].piece = WPAWN;
	data[h2].has_piece = true;
	data[h2].piece = WPAWN;

	// RANK 3
	data[a3].has_piece = true;
	data[a3].piece = WPAWN;
	data[b3].has_piece = true;
	data[b3].piece = WPAWN;
	data[c3].has_piece = true;
	data[c3].piece = WKNIGHT;
	data[d3].has_piece = true;
	data[d3].piece = WBISHOP;
	data[f3].has_piece = true;
	data[f3].piece = WKNIGHT;
	data[g3].has_piece = true;
	data[g3].piece = WPAWN;
	data[h3].has_piece = true;
	data[h3].piece = BBISHOP;

	// RANK 4
	data[b4].has_piece = true;
	data[b4].piece = BBISHOP;
	data[c4].has_piece = true;
	data[c4].piece = WPAWN;
	data[e4].has_piece = true;
	data[e4].piece = WPAWN;

	// RANK 5
	data[a5].has_piece = true;
	data[a5].piece = BKNIGHT;
	data[b5].has_piece = true;
	data[b5].piece = BPAWN;
	data[d5].has_piece = true;
	data[d5].piece = BPAWN;
	data[e5].has_piece = true;
	data[e5].piece = BPAWN;
	data[g5].has_piece = true;
	data[g5].piece = BPAWN;

	// RANK 6
	data[f6].has_piece = true;
	data[f6].piece = BKNIGHT;

	// RANK 7
	data[a7].has_piece = true;
	data[a7].piece = BPAWN;
	data[c7].has_piece = true;
	data[c7].piece = BPAWN;
	data[d7].has_piece = true;
	data[d7].piece = BKING;
	data[e7].has_piece = true;
	data[e7].piece = BQUEEN;
	data[f7].has_piece = true;
	data[f7].piece = BPAWN;
	data[h7].has_piece = true;
	data[h7].piece = BPAWN;

	// RANK 8
	data[a8].has_piece = true;
	data[a8].piece = BROOK;
	data[h8].has_piece = true;
	data[h8].piece = BROOK;


	bool found = false;
	enum piece pce;
	for ( int s = a1; s <= h8; s++) {
		struct sq_pce d = data[s];

		bool b = try_get_piece_on_sq(brd, s, &pce);

		if (b == true) {
			assert_true(pce == d.piece);
		}
	}

}

static void test_fen_side_to_move(void)
{
	struct parsed_fen* brd = parse_fen(RANDOM_FEN_1);
	enum colour side = get_side_to_move(brd);
	assert_true(side == WHITE);

	brd = parse_fen(RANDOM_FEN_2);
	side = get_side_to_move(brd);
	assert_true(side == BLACK);
}

static void test_fen_castle_permissions_initial_fen(void)
{
	struct parsed_fen* brd = parse_fen(RANDOM_FEN_1);

	cast_perm_t perm = CAST_PERM_WK;
	bool b = has_cast_perm(brd, perm);
	assert_true(b);

	perm = CAST_PERM_WQ;
	b = has_cast_perm(brd, perm);
	assert_true(b);

	perm = CAST_PERM_BQ;
	b = has_cast_perm(brd, perm);
	assert_true(b);

	perm = CAST_PERM_BK;
	b = has_cast_perm(brd, perm);
	assert_true(b);
}

static void test_fen_half_move_count(void)
{

}

static void test_fen_full_move_count(void)
{

}

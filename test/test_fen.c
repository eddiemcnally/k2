#include <stdbool.h>
#include <stdint.h>
#include "seatest.h"
#include "piece.h"
#include "position.h"
#include "square.h"
#include "fen.h"
#include "test_fen.h"


const char *fen_starting_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
static void test_fen_pieces_init_position(void);


void test_fixture_fen(void)
{
	test_fixture_start();               // starts a fixture

	run_test(test_fen_pieces_init_position);

	test_fixture_end();                 // ends a fixture
}





static void test_fen_pieces_init_position(void)
{

	struct parsed_fen* brd = parse_fen(fen_starting_position);

	bool found = false;
	enum piece pce;

	found = try_get_piece_on_sq(brd, a1, &pce);
	assert_true(found);
	assert_true(pce == WPAWN);
}



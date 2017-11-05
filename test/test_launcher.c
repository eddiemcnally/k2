
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "test_fen.h"
#include "test_move.h"
#include "test_piece.h"
#include "test_position.h"
#include "test_square.h"


int main(void)
{
	const struct CMUnitTest tests[] = {
		// FEN Testing
		cmocka_unit_test(test_fen_pieces_init_position),
		cmocka_unit_test(test_fen_pieces_random_position),
		cmocka_unit_test(test_fen_side_to_move),
		cmocka_unit_test(test_fen_castle_permissions_initial_fen),
		cmocka_unit_test(test_fen_castle_permissions_random_fen),
		cmocka_unit_test(test_fen_half_move_count),
		cmocka_unit_test(test_fen_full_move_count),
		cmocka_unit_test(test_fen_en_passant),

		// Moves
		cmocka_unit_test(test_move_quiet_move_encode_decode),

		// Pieces
		cmocka_unit_test(test_piece_is_white),
		cmocka_unit_test(test_piece_is_black),
		cmocka_unit_test(test_piece_get_colour_white_pieces),
		cmocka_unit_test(test_piece_get_colour_black_pieces),
		cmocka_unit_test(test_piece_swap_side),
		cmocka_unit_test(test_piece_get_piece_label),
		cmocka_unit_test(test_piece_values),
		cmocka_unit_test(test_piece_get_piece_from_label),

		// Position
		cmocka_unit_test(test_position_add_castle_perm),
		cmocka_unit_test(test_position_remove_castle_perm),



	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}


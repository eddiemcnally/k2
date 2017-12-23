// Copyright (c) 2017 Eddie McNally
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "test_fen.h"
#include "test_move.h"
#include "test_piece.h"
#include "test_position.h"
#include "test_square.h"
#include "test_bitboard.h"
#include "test_move_list.h"


int main(void)
{
	const struct CMUnitTest tests[] =
	{
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

		// bitboard
		cmocka_unit_test(test_bitboard_pop_first_bit_random_squares),
		cmocka_unit_test(test_bitboard_pop_first_bit_all_squares),
		cmocka_unit_test(test_bitboard_count_bits),
		cmocka_unit_test(test_bitboard_set_test_clear_bit),

		// move list
		cmocka_unit_test(test_move_list_init),
		cmocka_unit_test(test_move_list_bulk_add_moves),



	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}


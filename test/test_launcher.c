/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */


#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "test_fen.h"
#include "test_move.h"
#include "test_piece.h"
#include "test_position.h"
#include "test_square.h"
#include "test_board.h"
#include "test_bitboard.h"
#include "test_castle_permissions.h"
#include "test_move_list.h"
#include "test_move_gen.h"


int main ( void )
{


        const struct CMUnitTest tests[] = {
                // FEN Testing
                cmocka_unit_test ( test_fen_pieces_init_position ),
                cmocka_unit_test ( test_fen_pieces_random_position ),
                cmocka_unit_test ( test_fen_side_to_move ),
                cmocka_unit_test ( test_fen_castle_permissions_initial_fen ),
                cmocka_unit_test ( test_fen_castle_permissions_random_fen ),
                cmocka_unit_test ( test_fen_half_move_count ),
                cmocka_unit_test ( test_fen_full_move_count ),
                cmocka_unit_test ( test_fen_en_passant ),

                // Moves
                cmocka_unit_test ( test_move_quiet_move_encode_decode ),
                cmocka_unit_test ( test_move_promoted_capture_move_encode_decode ),
                cmocka_unit_test ( test_move_promoted_non_capture_move_encode_decode ),
                cmocka_unit_test ( test_move_en_passant_move_encode_decode ),


                // move list
                cmocka_unit_test ( test_move_list_init ),
                cmocka_unit_test ( test_move_list_bulk_add_moves ),
                cmocka_unit_test ( test_move_list_reset_list ),
                cmocka_unit_test ( test_move_list_contains_move ),

                // Pieces
                cmocka_unit_test ( test_piece_is_white ),
                cmocka_unit_test ( test_piece_is_black ),
                cmocka_unit_test ( test_piece_get_colour_white_pieces ),
                cmocka_unit_test ( test_piece_get_colour_black_pieces ),
                cmocka_unit_test ( test_piece_swap_side ),
                cmocka_unit_test ( test_piece_get_piece_label ),
                cmocka_unit_test ( test_piece_values ),
                cmocka_unit_test ( test_piece_get_piece_from_label ),

                // Position
                cmocka_unit_test ( test_position_get_set_castle_permissions ),


                // bitboard
                cmocka_unit_test ( test_bitboard_pop_first_bit_random_squares ),
                cmocka_unit_test ( test_bitboard_pop_first_bit_all_squares ),
                cmocka_unit_test ( test_bitboard_count_bits ),
                cmocka_unit_test ( test_bitboard_set_test_clear_bit ),
                cmocka_unit_test ( test_bitboard_reverse_bits ),

                // board
                cmocka_unit_test ( test_board_brd_allocate_deallocate ),
                cmocka_unit_test ( test_board_brd_bulk_add_remove_piece ),
                cmocka_unit_test ( test_board_brd_move_piece ),
                cmocka_unit_test ( test_board_brd_get_piece_bb ),
                cmocka_unit_test ( test_board_brd_get_colour_bb_black ),
                cmocka_unit_test ( test_board_brd_get_colour_bb_white ),
                cmocka_unit_test ( test_board_brd_is_sq_occupied ),
                cmocka_unit_test ( test_board_brd_try_get_piece_on_square ),
                cmocka_unit_test ( test_board_brd_get_board_bb ),

                // square
                cmocka_unit_test ( test_square_sq_get_rank ),
                cmocka_unit_test ( test_square_sq_get_file ),
                cmocka_unit_test ( test_square_gen_from_rank_file ),

                // move generation
                cmocka_unit_test ( test_move_gen_knight_white ),
                cmocka_unit_test ( test_move_gen_knight_black ),
                cmocka_unit_test ( test_move_gen_sliding_diagonal_black ),

                // castle permissions
                cmocka_unit_test ( test_castle_permissions_get_set ),
                cmocka_unit_test ( test_castle_permissions_no_perms_get_set ),


        };

        return cmocka_run_group_tests ( tests, NULL, NULL );
}

// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

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

#include "test_launcher.h"
#include "cmocka.h"
#include "test_attack_checker.h"
#include "test_basic_evaluator.h"
#include "test_bitboard.h"
#include "test_board.h"
#include "test_castle_permissions.h"
#include "test_fen.h"
#include "test_hashkeys.h"
#include "test_move.h"
#include "test_move_gen.h"
#include "test_move_list.h"
#include "test_piece.h"
#include "test_position.h"
#include "test_position_hist.h"
#include "test_square.h"
#include "test_transposition_table.h"
#include <setjmp.h>

#define TEST(test_name) cmocka_unit_test(test_name)

int main(void) {

    const struct CMUnitTest tests[] = {
        // FEN Testing
        TEST(test_fen_pieces_init_position),
        TEST(test_fen_pieces_random_position_1),
        TEST(test_fen_side_to_move),
        TEST(test_fen_castle_permissions_initial_fen),
        TEST(test_fen_castle_permissions_random_fen),
        TEST(test_fen_half_move_count),
        TEST(test_fen_full_move_count),
        TEST(test_fen_en_passant),

        // Moves
        TEST(test_move_quiet_move_encode_decode),
        TEST(test_move_promoted_capture_move_encode_decode),
        TEST(test_move_promoted_non_capture_move_encode_decode),
        TEST(test_move_en_passant_move_encode_decode),
        TEST(test_move_is_double_pawn_white),
        TEST(test_move_is_double_pawn_black),
        TEST(test_white_king_castle_encode_and_test),
        TEST(test_white_queen_castle_encode_and_test),
        TEST(test_black_king_castle_encode_and_test),
        TEST(test_black_queen_castle_encode_and_test),
        TEST(test_move_quiet_encode_decode),
        TEST(test_move_capture_encode_decode),
        TEST(test_move_white_queen_castle_encode_decode),
        TEST(test_move_white_king_castle_encode_decode),
        TEST(test_move_black_queen_castle_encode_decode),
        TEST(test_move_black_king_castle_encode_decode),

        TEST(test_move_double_pawn_move_encode_decode),

        // move list
        TEST(test_move_list_init),
        TEST(test_move_list_bulk_add_moves),
        TEST(test_move_list_reset_list),
        TEST(test_move_list_contains_move),
        TEST(test_move_list_compare),

        // Pieces
        TEST(test_piece_is_white),
        TEST(test_piece_is_black),
        TEST(test_piece_get_colour_white_pieces),
        TEST(test_piece_get_colour_black_pieces),
        TEST(test_piece_swap_side),
        TEST(test_piece_values),
        TEST(test_piece_get_piece_from_label),
        TEST(test_piece_get_array_idx),
        TEST(test_piece_get_piece_label),
        TEST(test_piece_role),
        TEST(test_piece_is_king),

        // Position
        TEST(test_position_get_set_castle_permissions),
        TEST(test_position_brd_is_sq_occupied),
        TEST(test_position_compare),
        TEST(test_position_make_move_white_double_first_move),
        TEST(test_position_make_move_black_double_first_move),
        TEST(test_position_make_move_castle_white_kingside_move_valid_position_updated),
        TEST(test_position_make_move_castle_white_queenside_move_valid_position_updated),
        TEST(test_position_make_move_castle_black_kingside_move_valid_position_updated),
        TEST(test_position_make_move_castle_black_queenside_move_valid_position_updated),
        TEST(test_move_history_push_multiple_moves_used_slots_as_expected),
        TEST(test_position_make_move_castle_white_kingside_move_invalid),
        TEST(test_position_make_move_castle_black_kingside_move_invalid),
        TEST(test_position_make_move_castle_white_queenside_move_invalid),
        TEST(test_position_make_move_castle_black_queenside_move_invalid),
        TEST(test_position_make_move_white_knight),
        TEST(test_position_make_move_black_knight),
        TEST(test_position_make_move_white_bishop),
        TEST(test_position_make_move_black_bishop),
        TEST(test_position_make_move_black_queen),
        TEST(test_position_make_move_white_queen),
        TEST(test_position_make_move_white_discovered_attack_on_king_invalid_move),
        TEST(test_position_make_move_black_discovered_attack_on_king_invalid_move),
        TEST(test_position_make_move_white_king_valid_moves),
        TEST(test_position_make_move_white_king_invalid_moves),
        TEST(test_position_make_move_black_king_valid_moves),
        TEST(test_position_make_move_black_king_invalid_moves),
        TEST(test_position_make_move_black_en_passant),
        TEST(test_position_make_move_white_en_passant),
        TEST(test_position_make_move_white_promotion),
        TEST(test_position_make_move_black_promotion),
        TEST(test_position_make_move_white_promotion_capture),
        TEST(test_position_make_move_black_promotion_capture),
        TEST(test_position_make_move_then_take_move_positions_restored_as_expected),
        TEST(test_position_make_move_sparse_board_black_to_move),
        TEST(test_position_make_take_double_pawn_move_white),
        TEST(test_position_make_take_quiet_move_white),
        TEST(test_position_make_take_double_pawn_move_black),
        TEST(test_position_make_take_quiet_move_black),
        TEST(test_position_make_take_capture_move_black),
        TEST(test_position_make_take_capture_move_white),
        TEST(test_position_make_take_en_passant_white_capture),
        TEST(test_position_make_take_en_passant_black_capture),
        TEST(test_position_make_take_king_castle_white),
        TEST(test_position_make_take_queen_castle_white),
        TEST(test_position_make_take_king_castle_black),
        TEST(test_position_make_take_queen_castle_black),
        TEST(test_position_make_take_quiet_promotion_white),
        TEST(test_position_make_take_quiet_promotion_black),
        TEST(test_position_make_take_capture_promotion_white),
        TEST(test_position_make_take_capture_promotion_black),

        TEST(test_position_hash_updated_white_pawn_quiet_move),
        TEST(test_position_hash_updated_black_pawn_quiet_move),
        TEST(test_position_hash_updated_white_pawn_capture_move),
        TEST(test_position_hash_updated_black_pawn_capture_move),
        TEST(test_position_hash_updated_white_pawn_double_first_move),
        TEST(test_position_hash_updated_black_pawn_double_first_move),
        TEST(test_position_hash_updated_black_en_passant),
        TEST(test_position_hash_updated_white_en_passant),
        TEST(test_position_hash_updated_white_promotion_quiet),
        TEST(test_position_hash_updated_black_promotion_quiet),
        TEST(test_position_hash_updated_white_promotion_capture),
        TEST(test_position_hash_updated_black_promotion_capture),
        TEST(test_position_hash_updated_white_move_quiet),
        TEST(test_position_hash_updated_white_move_capture),
        TEST(test_position_hash_updated_black_move_quiet),
        TEST(test_position_hash_updated_black_move_capture),
        TEST(test_position_hash_updated_white_king_castle),
        TEST(test_position_hash_updated_white_queen_castle),
        TEST(test_position_hash_updated_black_king_castle),
        TEST(test_position_hash_updated_black_queen_castle),

        // position evaluation
        TEST(test_basic_evaluator_sample_white_position),
        TEST(test_basic_evaluator_sample_black_position),

        // search
        TEST(test_transposition_table_create_different_sizes_as_expected),
        TEST(test_transposition_table_add_multiple_all_present),

        // hashkey mgmt
        TEST(test_hashkeys_init_to_non_zero_value),
        TEST(test_hashkeys_all_pieces_all_squares_before_after_same),
        TEST(test_hashkeys_update_side),
        TEST(test_hashkeys_update_castle_permissions),
        TEST(test_hashkeys_all_en_passant_squares_before_after_same),

        // bitboard
        TEST(test_bitboard_pop_first_bit_random_squares),
        TEST(test_bitboard_pop_first_bit_all_squares),
        TEST(test_bitboard_set_test_clear_bit),
        TEST(test_bitboard_reverse_bits),

        // board
        TEST(test_board_brd_allocate_deallocate),
        TEST(test_board_brd_bulk_add_remove_piece),
        TEST(test_board_brd_move_piece),
        TEST(test_board_brd_get_piece_bb),
        TEST(test_board_brd_get_colour_bb_black),
        TEST(test_board_brd_get_colour_bb_white),
        TEST(test_board_brd_is_sq_occupied),
        TEST(test_board_brd_get_piece_on_square),
        TEST(test_board_brd_get_piece_on_square_1),
        TEST(test_board_brd_get_board_bb),
        TEST(test_board_compare),

        // square
        TEST(test_square_sq_get_rank),
        TEST(test_square_sq_get_file),
        TEST(test_square_gen_from_rank_file),
        TEST(test_sq_get_square_plus_1_rank),
        TEST(test_sq_get_square_minus_1_rank),
        TEST(test_sq_try_get_sq_rank_and_file_invalid_returns_false),
        TEST(test_sq_try_get_sq_rank_and_file_valid_returns_true),

        // move generation
        TEST(test_move_gen_knight_white_1),
        TEST(test_move_gen_knight_white_2),
        TEST(test_move_gen_knight_white_3),
        TEST(test_move_gen_knight_black_1),
        TEST(test_move_gen_knight_black_2),
        TEST(test_move_gen_knight_black_3),

        TEST(test_move_gen_king_white_no_castling_1),
        TEST(test_move_gen_king_white_no_castling_2),
        TEST(test_move_gen_king_white_no_castling_3),
        TEST(test_move_gen_king_black_no_castling_1),
        TEST(test_move_gen_king_black_no_castling_2),
        TEST(test_move_gen_king_black_no_castling_3),

        TEST(test_move_white_castling_WK_WQ),
        TEST(test_move_white_castling_WK_only),
        TEST(test_move_white_castling_WQ_only),
        TEST(test_move_black_castling_BK_BQ),
        TEST(test_move_black_castling_BK_only),
        TEST(test_move_black_castling_BQ_only),

        TEST(test_move_white_pawns_promotion_1),
        TEST(test_move_white_pawns_promotion_2),
        TEST(test_move_white_pawns_1),
        TEST(test_move_white_pawns_2),
        TEST(test_move_white_pawns_first_move_double_1),
        TEST(test_move_white_pawns_first_move_double_2),
        TEST(test_move_white_pawns_en_passant_1),
        TEST(test_move_white_pawns_en_passant_2),
        TEST(test_move_white_pawns_en_passant_3),

        TEST(test_move_black_pawns_promotion_1),
        TEST(test_move_black_pawns_promotion_2),
        TEST(test_move_black_pawns_1),
        TEST(test_move_black_pawns_2),
        TEST(test_move_black_pawns_first_move_double_1),
        TEST(test_move_black_pawns_first_move_double_2),
        TEST(test_move_black_pawns_en_passant_1),
        TEST(test_move_black_pawns_en_passant_2),
        TEST(test_move_black_pawns_en_passant_3),

        TEST(test_move_white_bishop_1),
        TEST(test_move_white_bishop_2),
        TEST(test_move_white_bishop_3),
        TEST(test_move_black_bishop_1),
        TEST(test_move_black_bishop_2),

        TEST(test_move_white_rook_1),
        TEST(test_move_white_rook_2),
        TEST(test_move_white_rook_3),
        TEST(test_move_black_rook_1),
        TEST(test_move_white_queen_1),
        TEST(test_move_black_queen_1),

        TEST(test_move_all_moves_4_knights_opening_white_to_move),
        TEST(test_move_white_capture_only_moves),
        TEST(test_move_black_capture_only_moves),

        TEST(test_move_set_get_score),

        // attack checker
        TEST(test_att_chk_is_white_pawn_attacking),
        TEST(test_att_chk_is_black_pawn_attacking),
        TEST(test_att_chk_is_white_knight_attacking),
        TEST(test_att_chk_is_black_knight_attacking),
        TEST(test_att_chk_is_white_king_attacking),
        TEST(test_att_chk_is_black_king_attacking),
        TEST(test_att_chk_is_white_horizontal_and_vertical_attacking),
        TEST(test_att_chk_is_black_horizontal_and_vertical_attacking),
        TEST(test_att_chk_is_white_diagonal_attacking),
        TEST(test_att_chk_is_black_diagonal_attacking),

        // castle permissions
        TEST(test_castle_permissions_get_set),
        TEST(test_castle_permissions_no_perms_get_set),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

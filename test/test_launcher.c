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

#include "test_launcher.h"
#include "test_bitboard.h"
#include "test_board.h"
#include "test_castle_permissions.h"
#include "test_fen.h"
#include "test_move.h"
#include "test_move_gen.h"
#include "test_move_list.h"
#include "test_piece.h"
#include "test_position.h"
#include "test_square.h"
#include <cmocka.h>
#include <setjmp.h>

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
        cmocka_unit_test(test_move_promoted_capture_move_encode_decode),
        cmocka_unit_test(test_move_promoted_non_capture_move_encode_decode),
        cmocka_unit_test(test_move_en_passant_move_encode_decode),
        cmocka_unit_test(test_move_decode_promotion_piece_white),
        cmocka_unit_test(test_move_decode_promotion_piece_black),
        cmocka_unit_test(test_move_is_double_pawn_white),
        cmocka_unit_test(test_move_is_double_pawn_black),
        cmocka_unit_test(test_move_get_promote_piece_white),
        cmocka_unit_test(test_move_get_promote_piece_black),
        cmocka_unit_test(test_king_castle_encode_and_test),
        cmocka_unit_test(test_queen_castle_encode_and_test),

        // move list
        cmocka_unit_test(test_move_list_init),
        cmocka_unit_test(test_move_list_bulk_add_moves),
        cmocka_unit_test(test_move_list_reset_list),
        cmocka_unit_test(test_move_list_contains_move),
        cmocka_unit_test(test_move_list_compare),

        // Pieces
        cmocka_unit_test(test_piece_is_white),
        cmocka_unit_test(test_piece_is_black),
        cmocka_unit_test(test_piece_get_colour_white_pieces),
        cmocka_unit_test(test_piece_get_colour_black_pieces),
        cmocka_unit_test(test_piece_swap_side),
        cmocka_unit_test(test_piece_get_piece_label),
        cmocka_unit_test(test_piece_values),
        cmocka_unit_test(test_piece_get_piece_from_label),
        cmocka_unit_test(test_piece_get_array_idx),

        // Position
        cmocka_unit_test(test_position_get_set_castle_permissions),
        cmocka_unit_test(test_position_brd_is_sq_occupied),
        cmocka_unit_test(test_position_compare),
        cmocka_unit_test(test_position_white_double_first_move),
        cmocka_unit_test(test_position_black_double_first_move),
        cmocka_unit_test(test_castle_white_kingside_move_valid_position_updated),

        // bitboard
        cmocka_unit_test(test_bitboard_pop_first_bit_random_squares),
        cmocka_unit_test(test_bitboard_pop_first_bit_all_squares),
        cmocka_unit_test(test_bitboard_count_bits),
        cmocka_unit_test(test_bitboard_set_test_clear_bit),
        cmocka_unit_test(test_bitboard_reverse_bits),
        cmocka_unit_test(test_bb_get_sq_mask),

        // board
        cmocka_unit_test(test_board_brd_allocate_deallocate),
        cmocka_unit_test(test_board_brd_bulk_add_remove_piece),
        cmocka_unit_test(test_board_brd_move_piece),
        cmocka_unit_test(test_board_brd_get_piece_bb),
        cmocka_unit_test(test_board_brd_get_colour_bb_black),
        cmocka_unit_test(test_board_brd_get_colour_bb_white),
        cmocka_unit_test(test_board_brd_is_sq_occupied),
        cmocka_unit_test(test_board_brd_try_get_piece_on_square),
        cmocka_unit_test(test_board_brd_get_board_bb),
        cmocka_unit_test(test_board_compare),
        cmocka_unit_test(test_board_snapshot),

        // square
        cmocka_unit_test(test_square_sq_get_rank),
        cmocka_unit_test(test_square_sq_get_file),
        cmocka_unit_test(test_square_gen_from_rank_file),
        cmocka_unit_test(test_sq_get_square_plus_1_rank),
        cmocka_unit_test(test_sq_get_square_minus_1_rank),

        // move generation
        cmocka_unit_test(test_move_gen_knight_white_1),
        cmocka_unit_test(test_move_gen_knight_white_2),
        cmocka_unit_test(test_move_gen_knight_white_3),
        cmocka_unit_test(test_move_gen_knight_black_1),
        cmocka_unit_test(test_move_gen_knight_black_2),
        cmocka_unit_test(test_move_gen_knight_black_3),

        cmocka_unit_test(test_move_gen_king_white_no_castling_1),
        cmocka_unit_test(test_move_gen_king_white_no_castling_2),
        cmocka_unit_test(test_move_gen_king_white_no_castling_3),
        cmocka_unit_test(test_move_gen_king_black_no_castling_1),
        cmocka_unit_test(test_move_gen_king_black_no_castling_2),
        cmocka_unit_test(test_move_gen_king_black_no_castling_3),

        cmocka_unit_test(test_move_white_castling_WK_WQ),
        cmocka_unit_test(test_move_white_castling_WK_only),
        cmocka_unit_test(test_move_white_castling_WQ_only),
        cmocka_unit_test(test_move_black_castling_BK_BQ),
        cmocka_unit_test(test_move_black_castling_BK_only),
        cmocka_unit_test(test_move_black_castling_BQ_only),

        cmocka_unit_test(test_move_white_pawns_promotion_1),
        cmocka_unit_test(test_move_white_pawns_promotion_2),
        cmocka_unit_test(test_move_white_pawns_1),
        cmocka_unit_test(test_move_white_pawns_2),
        cmocka_unit_test(test_move_white_pawns_first_move_double_1),
        cmocka_unit_test(test_move_white_pawns_first_move_double_2),
        cmocka_unit_test(test_move_white_pawns_en_passant_1),
        cmocka_unit_test(test_move_white_pawns_en_passant_2),
        cmocka_unit_test(test_move_white_pawns_en_passant_3),

        cmocka_unit_test(test_move_black_pawns_promotion_1),
        cmocka_unit_test(test_move_black_pawns_promotion_2),
        cmocka_unit_test(test_move_black_pawns_1),
        cmocka_unit_test(test_move_black_pawns_2),
        cmocka_unit_test(test_move_black_pawns_first_move_double_1),
        cmocka_unit_test(test_move_black_pawns_first_move_double_2),
        cmocka_unit_test(test_move_black_pawns_en_passant_1),
        cmocka_unit_test(test_move_black_pawns_en_passant_2),
        cmocka_unit_test(test_move_black_pawns_en_passant_3),

        cmocka_unit_test(test_move_white_bishop_1),
        cmocka_unit_test(test_move_white_bishop_2),
        cmocka_unit_test(test_move_white_bishop_3),
        cmocka_unit_test(test_move_black_bishop_1),
        cmocka_unit_test(test_move_black_bishop_2),
        cmocka_unit_test(test_move_black_bishop_3),

        cmocka_unit_test(test_move_white_rook_1),
        cmocka_unit_test(test_move_white_rook_2),
        cmocka_unit_test(test_move_white_rook_3),
        cmocka_unit_test(test_move_black_rook_1),
        cmocka_unit_test(test_move_black_rook_2),
        cmocka_unit_test(test_move_black_rook_3),

        cmocka_unit_test(test_move_white_queen_1),
        cmocka_unit_test(test_move_black_queen_1),

        cmocka_unit_test(test_move_all_moves_4_knights_opening_white_to_move),

        // castle permissions
        cmocka_unit_test(test_castle_permissions_get_set),
        cmocka_unit_test(test_castle_permissions_no_perms_get_set),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

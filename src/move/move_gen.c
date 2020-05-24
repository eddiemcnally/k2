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

/*! @addtogroup MoveGeneration
 *
 * @ingroup MoveGeneration
 * @{
 * @details Provides functions to generate all moves from a given board and position.
 *
 */

#include "move_gen.h"
#include "bitboard.h"
#include "board.h"
#include "castle_perms.h"
#include "move.h"
#include "move_list.h"
#include "occupancy_mask.h"
#include "piece.h"
#include "position.h"
#include <assert.h>

// Bitboards representing commonly used ranks
#define RANK_2_BB ((uint64_t)0x000000000000FF00)
#define RANK_7_BB ((uint64_t)0x00FF000000000000)

// bitboards for squares between castle squares
#define CASTLE_MASK_WK ((uint64_t)0x0000000000000060)
#define CASTLE_MASK_WQ ((uint64_t)0x000000000000000E)
#define CASTLE_MASK_BK ((uint64_t)0x6000000000000000)
#define CASTLE_MASK_BQ ((uint64_t)0x0E00000000000000)

////////////////////////// start
static void mv_gen_white_pawn_moves(const struct position *pos,
                                    const struct board *brd,
                                    struct move_list *mvl);
static void mv_gen_black_pawn_moves(const struct position *pos,
                                    const struct board *brd,
                                    struct move_list *mvl);
static void get_sliding_diagonal_antidiagonal_moves(
    const struct board *brd, const uint64_t queen_bishop_bb,
    const enum colour side_to_move, struct move_list *mvl);
static void get_sliding_rank_file_moves(const struct board *brd,
                                        const uint64_t rook_queen_bb,
                                        const enum colour side_to_move,
                                        struct move_list *mvl);
static void encode_quiet_or_capt_move(const struct board *brd,
                                      const enum square from_sq,
                                      const enum square to_sq,
                                      struct move_list *mvl);
static void try_encode_double_pawn_move(const struct board *brd,
                                        const enum square from_sq,
                                        const enum square plus_1,
                                        const enum square plus_2,
                                        struct move_list *mvl);
static void gen_promotions(const enum square from_sq, const enum square to_sq,
                           struct move_list *mvl, const bool is_capture);
static void mv_gen_king_knight_moves(const struct board *brd,
                                     const enum colour colour,
                                     struct move_list *mvl);
static void mv_gen_encode_multiple_quiet(uint64_t bb, const enum square from_sq,
                                         struct move_list *mvl);
static void mv_gen_encode_multiple_capture(uint64_t bb,
                                           const enum square from_sq,
                                           struct move_list *mvl);
static void mv_gen_white_castle_moves(const struct position *pos,
                                      struct move_list *mvl);
static void mv_gen_black_castle_moves(const struct position *pos,
                                      struct move_list *mvl);
static void add_kingside_move_if_no_blockers(const uint64_t brd_bb,
                                             const uint64_t blocking_pce_mask,
                                             struct move_list *mvl,
                                             const enum colour side_to_move);
static void add_queenside_move_if_no_blockers(const uint64_t brd_bb,
                                              const uint64_t blocking_pce_mask,
                                              struct move_list *mvl,
                                              const enum colour side_to_move);

/**
 * @brief       Generates all valid moves for the given position
 * @details     Appends all new moves to the given move list
 * @param pos   The position
 * @param mvl   The move list
 */
void mv_gen_all_moves(const struct position *pos, struct move_list *mvl) {
    assert(validate_position(pos));
    assert(validate_move_list(mvl));

    struct board *brd = pos_get_board(pos);
    enum colour side_to_move = pos_get_side_to_move(pos);

    switch (side_to_move) {
    case WHITE:

        mv_gen_king_knight_moves(brd, WHITE, mvl);

        // conflate bishop and queen
        const uint64_t white_queen_bishop_bb =
            brd_get_piece_bb(brd, WHITE_QUEEN) |
            brd_get_piece_bb(brd, WHITE_BISHOP);
        get_sliding_diagonal_antidiagonal_moves(brd, white_queen_bishop_bb,
                                                WHITE, mvl);

        const uint64_t white_rook_queen_bb =
            brd_get_piece_bb(brd, WHITE_QUEEN) |
            brd_get_piece_bb(brd, WHITE_ROOK);
        get_sliding_rank_file_moves(brd, white_rook_queen_bb, WHITE, mvl);

        mv_gen_white_castle_moves(pos, mvl);
        mv_gen_white_pawn_moves(pos, brd, mvl);
        break;

    case BLACK:

        mv_gen_king_knight_moves(brd, BLACK, mvl);

        // conflate bishop and queen
        const uint64_t black_queen_bishop_bb =
            brd_get_piece_bb(brd, BLACK_QUEEN) |
            brd_get_piece_bb(brd, BLACK_BISHOP);
        get_sliding_diagonal_antidiagonal_moves(brd, black_queen_bishop_bb,
                                                BLACK, mvl);

        const uint64_t black_rook_queen_bb =
            brd_get_piece_bb(brd, BLACK_QUEEN) |
            brd_get_piece_bb(brd, BLACK_ROOK);
        get_sliding_rank_file_moves(brd, black_rook_queen_bb, BLACK, mvl);

        mv_gen_black_castle_moves(pos, mvl);
        mv_gen_black_pawn_moves(pos, brd, mvl);
        break;

    default:
        assert(false);
        break;
    }
}

/**
 * @brief       Generates White Pawn moves
 * @param brd   The board
 * @param mvl   the move list to append new moves to
 */
static void mv_gen_white_pawn_moves(const struct position *pos,
                                    const struct board *brd,
                                    struct move_list *mvl) {
    assert(validate_position(pos));
    assert(validate_board(brd));
    assert(validate_move_list(mvl));

    const uint64_t all_pawns_bb = brd_get_piece_bb(brd, WHITE_PAWN);
    const uint64_t black_bb = brd_get_colour_bb(brd, BLACK);
    const uint64_t all_occupied_squares_bb = brd_get_board_bb(brd);
    enum square en_pass_sq;
    const bool is_en_pass_active = pos_try_get_en_pass_sq(pos, &en_pass_sq);

    uint64_t pawns_excl_rank7_bb = all_pawns_bb & (~RANK_7_BB);

    while (pawns_excl_rank7_bb != 0) {
        // quiet moves
        const enum square from_sq = bb_pop_1st_bit(pawns_excl_rank7_bb);
        pawns_excl_rank7_bb = bb_clear_square(pawns_excl_rank7_bb, from_sq);

        const enum square to_sq = sq_get_square_plus_1_rank(from_sq);
        if (bb_is_set(all_occupied_squares_bb, to_sq) == false) {
            struct move quiet_move = move_encode_quiet(from_sq, to_sq);
            mvl_add(mvl, quiet_move);
        }

        // capture moves
        const uint64_t occ_mask =
            occ_mask_get_white_pawn_capture_non_first_double_move(from_sq);
        uint64_t capt_bb = black_bb & occ_mask;
        while (capt_bb != 0) {
            const enum square capt_to_sq = bb_pop_1st_bit(capt_bb);
            capt_bb = bb_clear_square(capt_bb, capt_to_sq);
            struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
            mvl_add(mvl, capt_move);
        }

        if (is_en_pass_active) {
            const uint64_t en_pass_bb = bb_get_sq_mask(en_pass_sq);
            if ((en_pass_bb & occ_mask) != 0) {
                // en passant move available from this square
                struct move en_pass_move =
                    move_encode_enpassant(from_sq, en_pass_sq);
                mvl_add(mvl, en_pass_move);
            }
        }
    }

    // now check for promotion
    uint64_t pawns_on_rank7_bb = all_pawns_bb & RANK_7_BB;
    while (pawns_on_rank7_bb != 0) {
        // quiet promotion
        const enum square prom_from_sq = bb_pop_1st_bit(pawns_on_rank7_bb);
        pawns_on_rank7_bb = bb_clear_square(pawns_on_rank7_bb, prom_from_sq);
        const enum square quiet_to_sq = sq_get_square_plus_1_rank(prom_from_sq);
        if (bb_is_set(all_occupied_squares_bb, quiet_to_sq) == false) {
            gen_promotions(prom_from_sq, quiet_to_sq, mvl, false);
        }

        // capture promotion
        const uint64_t occ_mask =
            occ_mask_get_white_pawn_capture_non_first_double_move(prom_from_sq);
        uint64_t capt_bb = black_bb & occ_mask;
        while (capt_bb != 0) {
            const enum square prom_capt_to_sq = bb_pop_1st_bit(capt_bb);
            capt_bb = bb_clear_square(capt_bb, prom_capt_to_sq);
            gen_promotions(prom_from_sq, prom_capt_to_sq, mvl, true);
        }
    }

    // mask out all pawns other than on rank 2
    uint64_t pawns_on_rank_2 = all_pawns_bb & RANK_2_BB;

    while (pawns_on_rank_2 != 0) {
        const enum square from_sq = bb_pop_1st_bit(pawns_on_rank_2);
        pawns_on_rank_2 = bb_clear_square(pawns_on_rank_2, from_sq);
        const enum square from_plus_1 = sq_get_square_plus_1_rank(from_sq);
        const enum square from_plus_2 = sq_get_square_plus_1_rank(from_plus_1);

        try_encode_double_pawn_move(brd, from_sq, from_plus_1, from_plus_2,
                                    mvl);
    }
}

/**
 * @brief       Generates Black Pawn moves
 * @param brd   The board
 * @param mvl   the move list to append new moves to
 */
static void mv_gen_black_pawn_moves(const struct position *pos,
                                    const struct board *brd,
                                    struct move_list *mvl) {
    assert(validate_position(pos));
    assert(validate_board(brd));
    assert(validate_move_list(mvl));

    const uint64_t all_pawns_bb = brd_get_piece_bb(brd, BLACK_PAWN);
    const uint64_t white_bb = brd_get_colour_bb(brd, WHITE);
    const uint64_t all_occupied_squares_bb = brd_get_board_bb(brd);
    enum square en_pass_sq;
    const bool is_en_pass_active = pos_try_get_en_pass_sq(pos, &en_pass_sq);

    uint64_t pawns_excl_rank2_bb = all_pawns_bb & (~RANK_2_BB);

    while (pawns_excl_rank2_bb != 0) {
        // quiet moves
        const enum square from_sq = bb_pop_1st_bit(pawns_excl_rank2_bb);
        pawns_excl_rank2_bb = bb_clear_square(pawns_excl_rank2_bb, from_sq);

        const enum square to_sq = sq_get_square_minus_1_rank(from_sq);
        if (bb_is_set(all_occupied_squares_bb, to_sq) == false) {
            struct move quiet_move = move_encode_quiet(from_sq, to_sq);
            mvl_add(mvl, quiet_move);
        }

        // capture moves
        const uint64_t occ_mask =
            occ_mask_get_black_pawn_capture_non_first_double_move(from_sq);
        uint64_t capt_bb = white_bb & occ_mask;
        while (capt_bb != 0) {
            const enum square capt_to_sq = bb_pop_1st_bit(capt_bb);
            capt_bb = bb_clear_square(capt_bb, capt_to_sq);

            struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
            mvl_add(mvl, capt_move);
        }

        if (is_en_pass_active) {
            const uint64_t en_pass_bb = bb_get_sq_mask(en_pass_sq);
            if ((en_pass_bb & occ_mask) != 0) {
                // en passant move available from this square
                struct move en_pass_move =
                    move_encode_enpassant(from_sq, en_pass_sq);
                mvl_add(mvl, en_pass_move);
            }
        }
    }

    // now check for promotion
    uint64_t pawns_on_rank2_bb = all_pawns_bb & RANK_2_BB;
    while (pawns_on_rank2_bb != 0) {
        // quiet promotion
        const enum square prom_from_sq = bb_pop_1st_bit(pawns_on_rank2_bb);
        pawns_on_rank2_bb = bb_clear_square(pawns_on_rank2_bb, prom_from_sq);

        const enum square prom_to_sq = sq_get_square_minus_1_rank(prom_from_sq);
        if (bb_is_set(all_occupied_squares_bb, prom_to_sq) == false) {
            gen_promotions(prom_from_sq, prom_to_sq, mvl, false);
        }

        // capture promotion
        const uint64_t occ_mask =
            occ_mask_get_black_pawn_capture_non_first_double_move(prom_from_sq);
        uint64_t capt_bb = white_bb & occ_mask;
        while (capt_bb != 0) {
            const enum square prom_capt_to_sq = bb_pop_1st_bit(capt_bb);
            capt_bb = bb_clear_square(capt_bb, prom_capt_to_sq);

            gen_promotions(prom_from_sq, prom_capt_to_sq, mvl, true);
        }
    }

    // mask out all pawns other than on rank 7
    uint64_t pawns_on_rank_7 = all_pawns_bb & RANK_7_BB;

    while (pawns_on_rank_7 != 0) {
        const enum square from_sq = bb_pop_1st_bit(pawns_on_rank_7);
        pawns_on_rank_7 = bb_clear_square(pawns_on_rank_7, from_sq);
        const enum square from_minus_1 = sq_get_square_minus_1_rank(from_sq);
        const enum square from_minus_2 =
            sq_get_square_minus_1_rank(from_minus_1);

        try_encode_double_pawn_move(brd, from_sq, from_minus_1, from_minus_2,
                                    mvl);
    }
}

static void get_sliding_diagonal_antidiagonal_moves(
    const struct board *brd, const uint64_t queen_bishop_bb,
    const enum colour side_to_move, struct move_list *mvl) {

    const uint64_t all_occupied_sq_bb = brd_get_board_bb(brd);
    const uint64_t col_occupied = brd_get_colour_bb(brd, side_to_move);

    uint64_t pce_to_move_bb = queen_bishop_bb;

    while (pce_to_move_bb != 0) {

        const enum square from_sq = bb_pop_1st_bit(pce_to_move_bb);
        pce_to_move_bb = bb_clear_square(pce_to_move_bb, from_sq);

        const uint64_t pos_diag_occ_mask =
            occ_mask_get_positive_diagonal(from_sq);
        const uint64_t neg_diag_occ_mask =
            occ_mask_get_negative_diagonal(from_sq);

        // create slider bb for this square
        const uint64_t bb_slider = bb_get_sq_mask(from_sq);

        // diagonal move
        uint64_t diag1 =
            (all_occupied_sq_bb & pos_diag_occ_mask) - (2 * bb_slider);
        uint64_t diag2 =
            bb_reverse(bb_reverse(all_occupied_sq_bb & pos_diag_occ_mask) -
                       (2 * bb_reverse(bb_slider)));
        const uint64_t diagpos = diag1 ^ diag2;

        // anti-diagonal moves
        diag1 = (all_occupied_sq_bb & neg_diag_occ_mask) - (2 * bb_slider);
        diag2 = bb_reverse(bb_reverse(all_occupied_sq_bb & neg_diag_occ_mask) -
                           (2 * bb_reverse(bb_slider)));
        const uint64_t diagneg = diag1 ^ diag2;

        const uint64_t all_moves =
            (diagpos & pos_diag_occ_mask) | (diagneg & neg_diag_occ_mask);

        // get all same colour as piece being considered
        uint64_t excl_same_col = all_moves & ~col_occupied;

        while (excl_same_col != 0) {
            enum square to_sq = bb_pop_1st_bit(excl_same_col);
            excl_same_col = bb_clear_square(excl_same_col, to_sq);

            encode_quiet_or_capt_move(brd, from_sq, to_sq, mvl);
        }
    }
}

static void get_sliding_rank_file_moves(const struct board *brd,
                                        const uint64_t rook_queen_bb,
                                        const enum colour side_to_move,
                                        struct move_list *mvl) {
    // taken from https://chessprogramming.wikispaces.com/Hyperbola+Quintessence

    const uint64_t all_occupied_sq_bb = brd_get_board_bb(brd);
    const uint64_t col_occupied = brd_get_colour_bb(brd, side_to_move);

    uint64_t pce_to_move_bb = rook_queen_bb;

    while (pce_to_move_bb != 0) {

        const enum square from_sq = bb_pop_1st_bit(pce_to_move_bb);
        pce_to_move_bb = bb_clear_square(pce_to_move_bb, from_sq);

        const uint64_t hmask = occ_mask_get_horizontal(from_sq);
        const uint64_t vmask = occ_mask_get_vertical(from_sq);

        // create slider bb for this square
        const uint64_t bb_slider = bb_get_sq_mask(from_sq);

        const uint64_t horiz1 = all_occupied_sq_bb - (2 * bb_slider);
        const uint64_t horiz2 = bb_reverse(bb_reverse(all_occupied_sq_bb) -
                                           2 * bb_reverse(bb_slider));
        const uint64_t horizontal = horiz1 ^ horiz2;

        const uint64_t vert1 = (all_occupied_sq_bb & vmask) - (2 * bb_slider);
        const uint64_t vert2 = bb_reverse(
            bb_reverse(all_occupied_sq_bb & vmask) - 2 * bb_reverse(bb_slider));
        const uint64_t vertical = vert1 ^ vert2;

        const uint64_t all_moves = (horizontal & hmask) | (vertical & vmask);

        uint64_t excl_same_col = all_moves & ~col_occupied;
        while (excl_same_col != 0) {
            enum square to_sq = bb_pop_1st_bit(excl_same_col);
            excl_same_col = bb_clear_square(excl_same_col, to_sq);
            encode_quiet_or_capt_move(brd, from_sq, to_sq, mvl);
        }
    }
}

static void encode_quiet_or_capt_move(const struct board *brd,
                                      const enum square from_sq,
                                      const enum square to_sq,
                                      struct move_list *mvl) {
    struct move mv;
    if (brd_is_sq_occupied(brd, to_sq)) {
        mv = move_encode_capture(from_sq, to_sq);
    } else {
        mv = move_encode_quiet(from_sq, to_sq);
    }

    mvl_add(mvl, mv);
}

static void try_encode_double_pawn_move(const struct board *brd,
                                        const enum square from_sq,
                                        const enum square plus_1,
                                        const enum square plus_2,
                                        struct move_list *mvl) {
    if (brd_is_sq_occupied(brd, plus_1) == false &&
        brd_is_sq_occupied(brd, plus_2) == false) {
        struct move quiet_move = move_encode_pawn_double_first(from_sq, plus_2);
        mvl_add(mvl, quiet_move);
    }
}

static void gen_promotions(const enum square from_sq, const enum square to_sq,
                           struct move_list *mvl, const bool is_capture) {

    struct move mv = move_encode_promoted(from_sq, to_sq, KNIGHT, is_capture);
    mvl_add(mvl, mv);

    mv = move_encode_promoted(from_sq, to_sq, BISHOP, is_capture);
    mvl_add(mvl, mv);

    mv = move_encode_promoted(from_sq, to_sq, ROOK, is_capture);
    mvl_add(mvl, mv);

    mv = move_encode_promoted(from_sq, to_sq, QUEEN, is_capture);
    mvl_add(mvl, mv);
}

/**
 * @brief Generates moves for King and Knight (ie, non-sliding pieces)
 *
 * @param brd The board
 * @param side_to_move The side to move
 * @param mvl A pointer to the move List, to which the moves are appended.
 */
static void mv_gen_king_knight_moves(const struct board *brd,
                                     const enum colour side_to_move,
                                     struct move_list *mvl) {

    const enum piece knight_pce =
        (side_to_move == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
    const enum piece king_pce =
        (side_to_move == WHITE) ? WHITE_KING : BLACK_KING;

    uint64_t knight_bb = brd_get_piece_bb(brd, knight_pce);
    uint64_t king_bb = brd_get_piece_bb(brd, king_pce);

    const enum colour opp_side = pce_swap_side(side_to_move);
    const uint64_t opp_colours_bb = brd_get_colour_bb(brd, opp_side);
    const uint64_t all_occupied_squares_bb = brd_get_board_bb(brd);
    const uint64_t free_squares = ~all_occupied_squares_bb;

    // knight
    while (knight_bb != 0) {
        const enum square from_sq = bb_pop_1st_bit(knight_bb);
        knight_bb = bb_clear_square(knight_bb, from_sq);

        const uint64_t occ_mask = occ_mask_get_knight(from_sq);

        // generate capture moves
        // ----------------------
        // AND'ing with opposite colour pieces with the occupancy mask, will
        // give all pieces that can be captured by the piece on this square
        const uint64_t captures_bb = occ_mask & opp_colours_bb;
        mv_gen_encode_multiple_capture(captures_bb, from_sq, mvl);

        // generate quiet moves
        // --------------------
        const uint64_t quiet_bb = free_squares & occ_mask;
        mv_gen_encode_multiple_quiet(quiet_bb, from_sq, mvl);
    }

    // king
    const enum square from_sq = bb_pop_1st_bit(king_bb);
    const uint64_t occ_mask = occ_mask_get_king(from_sq);

    // generate capture moves
    // ----------------------
    // AND'ing with opposite colour pieces with the occupancy mask, will
    // give all pieces that can be captured by the piece on this square
    const uint64_t captures_bb = occ_mask & opp_colours_bb;
    mv_gen_encode_multiple_capture(captures_bb, from_sq, mvl);

    // generate quiet moves
    // --------------------
    const uint64_t quiet_bb = free_squares & occ_mask;
    mv_gen_encode_multiple_quiet(quiet_bb, from_sq, mvl);
}

/**
 * @brief Generates quiet moves using the given bitboard and source square.
 *
 * @param bb A bitboard repesenting all possible non-occupied squares.
 * @param from_sq The source ("from") square
 * @param mvl Pointer to a move list. All generated moves are appended to this list.
 */
static void mv_gen_encode_multiple_quiet(uint64_t bb, const enum square from_sq,
                                         struct move_list *mvl) {
    while (bb != 0) {
        const enum square empty_sq = bb_pop_1st_bit(bb);
        bb = bb_clear_square(bb, empty_sq);

        const struct move quiet_move = move_encode_quiet(from_sq, empty_sq);
        mvl_add(mvl, quiet_move);
    }
}

/**
 * @brief Generates capture moves using the given bitboard and source square.
 *
 * @param bb A bitboard repesenting all possible capture squares.
 * @param from_sq The source ("from") square
 * @param mvl Pointer to a move list. All generated moves are appended to this list.
 */
static void mv_gen_encode_multiple_capture(uint64_t bb,
                                           const enum square from_sq,
                                           struct move_list *mvl) {
    while (bb != 0) {
        const enum square cap_sq = bb_pop_1st_bit(bb);
        bb = bb_clear_square(bb, cap_sq);
        const struct move cap_move = move_encode_capture(from_sq, cap_sq);
        mvl_add(mvl, cap_move);
    }
}

static void mv_gen_white_castle_moves(const struct position *pos,
                                      struct move_list *mvl) {
    const struct cast_perm_container cp = pos_get_cast_perm(pos);
    const struct board *brd = pos_get_board(pos);
    const uint64_t occupied_bb = brd_get_board_bb(brd);

    if (cast_perm_has_permission(CP_WK, cp)) {
        add_kingside_move_if_no_blockers(occupied_bb, CASTLE_MASK_WK, mvl,
                                         WHITE);
    }
    if (cast_perm_has_permission(CP_WQ, cp)) {
        add_queenside_move_if_no_blockers(occupied_bb, CASTLE_MASK_WQ, mvl,
                                          WHITE);
    }
}

static void mv_gen_black_castle_moves(const struct position *pos,
                                      struct move_list *mvl) {
    const struct cast_perm_container cp = pos_get_cast_perm(pos);
    const struct board *brd = pos_get_board(pos);
    const uint64_t occupied_bb = brd_get_board_bb(brd);

    if (cast_perm_has_permission(CP_BK, cp)) {
        add_kingside_move_if_no_blockers(occupied_bb, CASTLE_MASK_BK, mvl,
                                         BLACK);
    }
    if (cast_perm_has_permission(CP_BQ, cp)) {
        add_queenside_move_if_no_blockers(occupied_bb, CASTLE_MASK_BQ, mvl,
                                          BLACK);
    }
}

static void add_kingside_move_if_no_blockers(const uint64_t brd_bb,
                                             const uint64_t blocking_pce_mask,
                                             struct move_list *mvl,
                                             const enum colour side_to_move) {
    if ((brd_bb & blocking_pce_mask) == 0) {
        const struct move mv = move_encode_castle_kingside(side_to_move);
        mvl_add(mvl, mv);
    }
}

static void add_queenside_move_if_no_blockers(const uint64_t brd_bb,
                                              const uint64_t blocking_pce_mask,
                                              struct move_list *mvl,
                                              const enum colour side_to_move) {
    if ((brd_bb & blocking_pce_mask) == 0) {
        const struct move mv = move_encode_castle_queenside(side_to_move);
        mvl_add(mvl, mv);
    }
}

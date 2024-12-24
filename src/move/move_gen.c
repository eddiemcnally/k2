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
#include "utils.h"
#include <assert.h>

// bitboards for squares between castle squares
// clang-format off
#define CASTLE_SQ_MASK_WK ((uint64_t)(SQUARE_AS_BITBOARD(f1) | SQUARE_AS_BITBOARD(g1)))
#define CASTLE_SQ_MASK_WQ ((uint64_t)(SQUARE_AS_BITBOARD(b1) | SQUARE_AS_BITBOARD(c1) | SQUARE_AS_BITBOARD(d1)))
#define CASTLE_SQ_MASK_BK ((uint64_t)(SQUARE_AS_BITBOARD(f8) | SQUARE_AS_BITBOARD(g8)))
#define CASTLE_SQ_MASK_BQ ((uint64_t)(SQUARE_AS_BITBOARD(b8) | SQUARE_AS_BITBOARD(c8) | SQUARE_AS_BITBOARD(d8)))
// clang-format on

enum move_gen_type {
    ALL_MOVES,
    CAPTURE_ONLY,
};

static void mv_gen_white_pawn_moves(const struct position *const pos, struct move_list *const mvl,
                                    const enum move_gen_type gen_type, const uint64_t all_pce_bb);
static void mv_gen_black_pawn_moves(const struct position *const pos, struct move_list *const mvl,
                                    const enum move_gen_type gen_type, const uint64_t all_pce_bb);
static void get_sliding_rank_file_moves(const struct position *const pos, struct move_list *const mvl,
                                        const enum move_gen_type gen_type);
static void get_sliding_diagonal_antidiagonal_moves(const struct position *const pos, struct move_list *const mvl,
                                                    const enum move_gen_type gen_type);
static void gen_promotions(const enum square from_sq, const enum square to_sq, struct move_list *const mvl);
static void gen_promotions_with_capture(const enum square from_sq, const enum square to_sq,
                                        struct move_list *const mvl);
static void mv_gen_knight_moves(const struct position *const pos, struct move_list *const mvl,
                                const enum move_gen_type gen_type);
static void mv_gen_king_moves(const struct position *const pos, struct move_list *const mvl,
                              const enum move_gen_type gen_type);
static void mv_gen_encode_multiple_capture(uint64_t bb, const enum square from_sq, struct move_list *const mvl);
static void mv_gen_encode_multiple_quiet(uint64_t bb, const enum square from_sq, struct move_list *const mvl);
static void mv_gen_white_castle_moves(struct move_list *const mvl, const struct cast_perm_container castle_perms,
                                      const uint64_t all_pce_bb);
static void mv_gen_black_castle_moves(struct move_list *const mvl, const struct cast_perm_container castle_perms,
                                      const uint64_t all_pce_bb);
static void mv_gen_moves(const struct position *const pos, struct move_list *const mvl,
                         const enum move_gen_type gen_type);

/**
 * @brief       Generates only capture moves from the given position
 * @details     Appends all new moves to the given move list
 * @param pos   The position
 * @param mvl   The move list
 */
void mv_gen_only_capture_moves(const struct position *const pos, struct move_list *const mvl) {

    assert(validate_position(pos));
    assert(validate_move_list(mvl));

    mv_gen_moves(pos, mvl, CAPTURE_ONLY);
}

/**
 * @brief       Generates all valid moves for the given position
 * @details     Appends all new moves to the given move list
 * @param pos   The position
 * @param mvl   The move list
 */
void mv_gen_all_moves(const struct position *const pos, struct move_list *const mvl) {
    assert(validate_position(pos));
    assert(validate_move_list(mvl));

    mv_gen_moves(pos, mvl, ALL_MOVES);
}

static void mv_gen_moves(const struct position *const pos, struct move_list *const mvl,
                         const enum move_gen_type gen_type) {

    const struct board *const brd = pos_get_board(pos);
    const struct cast_perm_container castle_perms = pos_get_cast_perm(pos);
    const uint64_t all_pce_bb = brd_get_board_bb(brd);

    switch (pos_get_side_to_move(pos)) {
    case WHITE: {
        if (cast_perm_has_white_permissions(castle_perms) && (gen_type == ALL_MOVES)) {
            mv_gen_white_castle_moves(mvl, castle_perms, all_pce_bb);
        }

        mv_gen_white_pawn_moves(pos, mvl, gen_type, all_pce_bb);
    } break;

    case BLACK: {
        if (cast_perm_has_black_permissions(castle_perms) && (gen_type == ALL_MOVES)) {
            mv_gen_black_castle_moves(mvl, castle_perms, all_pce_bb);
        }

        mv_gen_black_pawn_moves(pos, mvl, gen_type, all_pce_bb);
    } break;
    default:
        print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Invalid side");
        break;
    }

    // common move code
    mv_gen_king_moves(pos, mvl, gen_type);
    mv_gen_knight_moves(pos, mvl, gen_type);
    get_sliding_diagonal_antidiagonal_moves(pos, mvl, gen_type);
    get_sliding_rank_file_moves(pos, mvl, gen_type);
}

static void mv_gen_white_pawn_moves(const struct position *const pos, struct move_list *const mvl,
                                    const enum move_gen_type gen_type, const uint64_t all_pce_bb) {
    assert(validate_position(pos));
    assert(validate_move_list(mvl));

    const struct board *const brd = pos_get_board(pos);

    uint64_t all_pawns_bb = brd_get_piece_bb(brd, WHITE_PAWN);
    const uint64_t black_pce_bb = brd_get_black_bb(brd);

    if (pos_is_en_passant_active(pos)) {
        const enum square en_pass_sq = pos_get_en_pass_sq(pos);
        const uint64_t att_squares_bb = occ_mask_get_bb_white_pawns_attacking_sq(en_pass_sq);
        uint64_t att_enp_pawn_bb = all_pawns_bb & att_squares_bb;
        while (att_enp_pawn_bb != 0) {
            const enum square from_sq = bb_pop_1st_bit_and_clear(&att_enp_pawn_bb);
            const struct move en_pass_move = move_encode_enpassant(from_sq, en_pass_sq);
            mvl_add(mvl, en_pass_move);
        }
    }

    while (all_pawns_bb != 0) {
        const enum square from_sq = bb_pop_1st_bit_and_clear(&all_pawns_bb);
        const enum rank pawn_rank = sq_get_rank(from_sq);

        switch (pawn_rank) {
        case RANK_2: {
            if (gen_type == ALL_MOVES) {
                // double first move
                const uint64_t double_bb = occ_mask_get_double_pawn_mask_white(from_sq);
                if ((double_bb & all_pce_bb) == 0) {
                    const enum square to_sq = sq_get_square_plus_2_rank(from_sq);
                    const struct move quiet_move = move_encode_pawn_double_first(from_sq, to_sq);
                    mvl_add(mvl, quiet_move);
                }

                // quiet move
                const enum square to_sq = sq_get_square_plus_1_rank(from_sq);
                if (bb_is_set(all_pce_bb, to_sq) == false) {
                    const struct move quiet_move = move_encode_quiet(from_sq, to_sq);
                    mvl_add(mvl, quiet_move);
                }
            }

            // capture moves
            const uint64_t occ_mask = occ_mask_get_white_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = black_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square capt_to_sq = bb_pop_1st_bit_and_clear(&capt_bb);
                const struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
                mvl_add(mvl, capt_move);
            }
        } break;
        case RANK_7: {
            // quiet promotion
            if (gen_type == ALL_MOVES) {
                const enum square quiet_to_sq = sq_get_square_plus_1_rank(from_sq);
                if (bb_is_set(all_pce_bb, quiet_to_sq) == false) {
                    gen_promotions(from_sq, quiet_to_sq, mvl);
                }
            }

            // capture promotion
            const uint64_t occ_mask = occ_mask_get_white_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = black_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square prom_capt_to_sq = bb_pop_1st_bit_and_clear(&capt_bb);
                gen_promotions_with_capture(from_sq, prom_capt_to_sq, mvl);
            }
        } break;
        case RANK_3:
        case RANK_4:
        case RANK_5:
        case RANK_6: {
            // quiet moves
            if (gen_type == ALL_MOVES) {
                const enum square to_sq = sq_get_square_plus_1_rank(from_sq);
                if (bb_is_set(all_pce_bb, to_sq) == false) {
                    const struct move quiet_move = move_encode_quiet(from_sq, to_sq);
                    mvl_add(mvl, quiet_move);
                }
            }

            // capture moves
            const uint64_t occ_mask = occ_mask_get_white_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = black_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square capt_to_sq = bb_pop_1st_bit_and_clear(&capt_bb);
                const struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
                mvl_add(mvl, capt_move);
            }
        } break;
        case RANK_1:
        case RANK_8:
            print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Unexpected pawn on Rank 1 or Rank 8");
            break;
        default:
            // invalid rank
            print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Unexpected rank for white");
            break;
        } //switch
    }
}

static void mv_gen_black_pawn_moves(const struct position *const pos, struct move_list *const mvl,
                                    const enum move_gen_type gen_type, const uint64_t all_pce_bb) {
    assert(validate_position(pos));
    assert(validate_move_list(mvl));

    const struct board *const brd = pos_get_board(pos);

    uint64_t all_pawns_bb = brd_get_piece_bb(brd, BLACK_PAWN);
    const uint64_t white_pce_bb = brd_get_white_bb(brd);

    if (pos_is_en_passant_active(pos)) {
        const enum square en_pass_sq = pos_get_en_pass_sq(pos);
        const uint64_t att_squares_bb = occ_mask_get_bb_black_pawns_attacking_sq(en_pass_sq);
        uint64_t att_enp_pawn_bb = all_pawns_bb & att_squares_bb;
        while (att_enp_pawn_bb != 0) {
            const enum square from_sq = bb_pop_1st_bit_and_clear(&att_enp_pawn_bb);
            const struct move en_pass_move = move_encode_enpassant(from_sq, en_pass_sq);
            mvl_add(mvl, en_pass_move);
        }
    }

    while (all_pawns_bb != 0) {
        const enum square from_sq = bb_pop_1st_bit_and_clear(&all_pawns_bb);
        const enum rank pawn_rank = sq_get_rank(from_sq);

        switch (pawn_rank) {
        case RANK_7: {
            if (gen_type == ALL_MOVES) {
                // double first move
                const uint64_t double_bb = occ_mask_get_double_pawn_mask_black(from_sq);
                if ((double_bb & all_pce_bb) == 0) {
                    const enum square to_sq = sq_get_square_minus_2_rank(from_sq);
                    const struct move quiet_move = move_encode_pawn_double_first(from_sq, to_sq);
                    mvl_add(mvl, quiet_move);
                }

                // quiet move
                const enum square to_sq = sq_get_square_minus_1_rank(from_sq);
                if (bb_is_set(all_pce_bb, to_sq) == false) {
                    const struct move quiet_move = move_encode_quiet(from_sq, to_sq);
                    mvl_add(mvl, quiet_move);
                }
            }

            // capture moves
            const uint64_t occ_mask = occ_mask_get_black_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = white_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square capt_to_sq = bb_pop_1st_bit_and_clear(&capt_bb);
                const struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
                mvl_add(mvl, capt_move);
            }
        } break;
        case RANK_2: {
            // quiet promotion
            if (gen_type == ALL_MOVES) {
                const enum square quiet_to_sq = sq_get_square_minus_1_rank(from_sq);
                if (bb_is_set(all_pce_bb, quiet_to_sq) == false) {
                    gen_promotions(from_sq, quiet_to_sq, mvl);
                }
            }

            // capture promotion
            const uint64_t occ_mask = occ_mask_get_black_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = white_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square prom_capt_to_sq = bb_pop_1st_bit_and_clear(&capt_bb);
                gen_promotions_with_capture(from_sq, prom_capt_to_sq, mvl);
            }
        } break;
        case RANK_3:
        case RANK_4:
        case RANK_5:
        case RANK_6: {
            // quiet moves
            if (gen_type == ALL_MOVES) {
                const enum square to_sq = sq_get_square_minus_1_rank(from_sq);
                if (bb_is_set(all_pce_bb, to_sq) == false) {
                    const struct move quiet_move = move_encode_quiet(from_sq, to_sq);
                    mvl_add(mvl, quiet_move);
                }
            }

            // capture moves
            const uint64_t occ_mask = occ_mask_get_black_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = white_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square capt_to_sq = bb_pop_1st_bit_and_clear(&capt_bb);
                const struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
                mvl_add(mvl, capt_move);
            }
        } break;
        case RANK_1:
        case RANK_8:
            print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Unexpected pawn on Rank 1 or Rank 8");
            break;
        default:
            // invalid rank
            print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Unexpected rank for black");
            break;
        } //switch
    }
}

/**
 * @brief Generate sliding diagonal and antidiagonal moves 
 * 
 * @param pos               The game position
 * @param mvl               The move list
 * @param gen_type          The move type to generate
 */
static void get_sliding_diagonal_antidiagonal_moves(const struct position *const pos, struct move_list *const mvl,
                                                    const enum move_gen_type gen_type) {

    assert((gen_type == CAPTURE_ONLY) || (gen_type == ALL_MOVES));

    const struct board *const brd = pos_get_board(pos);

    const enum colour side_to_move = pos_get_side_to_move(pos);
    const uint64_t occupied_sq_bb = brd_get_board_bb(brd);
    const uint64_t colour_bb = brd_get_colour_bb(brd, side_to_move);

    uint64_t pce_to_move_bb = brd_get_bishop_queen_bb_for_colour(brd, side_to_move);

    while (pce_to_move_bb != 0) {
        const enum square from_sq = bb_pop_1st_bit_and_clear(&pce_to_move_bb);

        const struct diagonals diag_masks = occ_mask_get_diagonals(from_sq);

        // create slider bb for this square
        const uint64_t bb_slider = SQUARE_AS_BITBOARD(from_sq);

        // diagonal move
        const uint64_t diag1 = (occupied_sq_bb & diag_masks.positive) - (bb_slider << 1);
        const uint64_t diag2 =
            bb_reverse(bb_reverse(occupied_sq_bb & diag_masks.positive) - (bb_reverse(bb_slider) << 1));
        const uint64_t diagpos = diag1 ^ diag2;

        // anti-diagonal moves
        const uint64_t antidiag1 = (occupied_sq_bb & diag_masks.negative) - (bb_slider << 1);
        const uint64_t antidiag2 =
            bb_reverse(bb_reverse(occupied_sq_bb & diag_masks.negative) - (bb_reverse(bb_slider) << 1));
        const uint64_t diagneg = antidiag1 ^ antidiag2;

        const uint64_t all_moves = (diagpos & diag_masks.positive) | (diagneg & diag_masks.negative);

        // get all same colour as piece being considered
        uint64_t excl_same_col = all_moves & ~colour_bb;

        while (excl_same_col != 0) {
            const enum square to_sq = bb_pop_1st_bit_and_clear(&excl_same_col);
            if (gen_type == ALL_MOVES) {
                if (bb_is_set(occupied_sq_bb, to_sq)) {
                    const struct move mv = move_encode_capture(from_sq, to_sq);
                    mvl_add(mvl, mv);
                } else {
                    const struct move mv = move_encode_quiet(from_sq, to_sq);
                    mvl_add(mvl, mv);
                }
            } else if (bb_is_set(occupied_sq_bb, to_sq)) {
                const struct move mv = move_encode_capture(from_sq, to_sq);
                mvl_add(mvl, mv);
            }
        }
    }
}

/**
 * @brief Generate sliding RANK and FILE moves 
 * 
 * @param pos               The game position
 * @param mvl               The move list
 * @param gen_type          The move type to generate
 */
static void get_sliding_rank_file_moves(const struct position *const pos, struct move_list *const mvl,
                                        const enum move_gen_type gen_type) {

    // taken from https://www.chessprogramming.org/Hyperbola_Quintessence

    const struct board *const brd = pos_get_board(pos);

    const enum colour side_to_move = pos_get_side_to_move(pos);
    const uint64_t occupied_sq_bb = brd_get_board_bb(brd);
    const uint64_t colour_bb = brd_get_colour_bb(brd, side_to_move);
    const uint64_t reversed_occupied_sq_bb = bb_reverse(occupied_sq_bb);

    uint64_t pce_to_move_bb = brd_get_rook_queen_bb_for_colour(brd, side_to_move);

    while (pce_to_move_bb != 0) {

        const enum square from_sq = bb_pop_1st_bit_and_clear(&pce_to_move_bb);

        const uint64_t hmask = occ_mask_get_horizontal(from_sq);
        const uint64_t vmask = occ_mask_get_vertical(from_sq);

        // create slider bb for this square
        const uint64_t bb_slider = SQUARE_AS_BITBOARD(from_sq);
        const uint64_t reverse_bb_slider = bb_reverse(bb_slider);

        const uint64_t horiz1 = occupied_sq_bb - (bb_slider << 1);
        const uint64_t horiz2 = bb_reverse(reversed_occupied_sq_bb - (reverse_bb_slider << 1));
        const uint64_t horizontal = horiz1 ^ horiz2;

        const uint64_t vert1 = (occupied_sq_bb & vmask) - (bb_slider << 1);
        const uint64_t vert2 = bb_reverse(bb_reverse(occupied_sq_bb & vmask) - (reverse_bb_slider << 1));
        const uint64_t vertical = vert1 ^ vert2;

        const uint64_t all_moves = (horizontal & hmask) | (vertical & vmask);

        uint64_t excl_same_col = all_moves & ~colour_bb;
        while (excl_same_col != 0) {
            const enum square to_sq = bb_pop_1st_bit_and_clear(&excl_same_col);
            if (gen_type == ALL_MOVES) {
                if (bb_is_set(occupied_sq_bb, to_sq)) {
                    const struct move mv = move_encode_capture(from_sq, to_sq);
                    mvl_add(mvl, mv);
                } else {
                    const struct move mv = move_encode_quiet(from_sq, to_sq);
                    mvl_add(mvl, mv);
                }
            } else if (bb_is_set(occupied_sq_bb, to_sq)) {
                const struct move mv = move_encode_capture(from_sq, to_sq);
                mvl_add(mvl, mv);
            }
        }
    }
}

static void gen_promotions(const enum square from_sq, const enum square to_sq, struct move_list *const mvl) {

    struct move mv = move_encode_promote_knight(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_bishop(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_rook(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_queen(from_sq, to_sq);
    mvl_add(mvl, mv);
}

static void gen_promotions_with_capture(const enum square from_sq, const enum square to_sq,
                                        struct move_list *const mvl) {

    struct move mv = move_encode_promote_knight_with_capture(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_bishop_with_capture(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_rook_with_capture(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_queen_with_capture(from_sq, to_sq);
    mvl_add(mvl, mv);
}

/**
 * @brief Generates Knight moves
 * 
 * @param pos           the game position
 * @param mvl           the move list
 * @param gen_type      the move types to generate
 */
static void mv_gen_knight_moves(const struct position *const pos, struct move_list *const mvl,
                                const enum move_gen_type gen_type) {

    assert((gen_type == CAPTURE_ONLY) || (gen_type == ALL_MOVES));

    const struct board *const brd = pos_get_board(pos);

    const enum colour side_to_move = pos_get_side_to_move(pos);
    const enum colour opposing_colour = pce_swap_side(side_to_move);

    const uint64_t occupied_sq_bb = brd_get_board_bb(brd);
    const uint64_t opposite_colour_bb = brd_get_colour_bb(brd, opposing_colour);

    uint64_t knight_bb = brd_get_bb_for_role_colour(brd, KNIGHT, side_to_move);
    while (knight_bb != 0) {
        const enum square from_sq = bb_pop_1st_bit_and_clear(&knight_bb);
        const uint64_t occ_mask = occ_mask_get_knight(from_sq);

        switch (gen_type) {
        case CAPTURE_ONLY:
            mv_gen_encode_multiple_capture((occ_mask & opposite_colour_bb), from_sq, mvl);
            break;
        case ALL_MOVES:
            // captures
            mv_gen_encode_multiple_capture((occ_mask & opposite_colour_bb), from_sq, mvl);
            // quiet moves
            mv_gen_encode_multiple_quiet((~occupied_sq_bb & occ_mask), from_sq, mvl);
            break;
        default:
            print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Invalid move gen type");
            break;
        }
    }
}

/**
 * @brief Generates King moves
 * 
 * @param pos           Current position
 * @param mvl           the move list
 * @param gen_type      the move types to generate
 */
static void mv_gen_king_moves(const struct position *const pos, struct move_list *const mvl,
                              const enum move_gen_type gen_type) {

    assert((gen_type == CAPTURE_ONLY) || (gen_type == ALL_MOVES));

    const struct board *const brd = pos_get_board(pos);

    const enum colour side_to_move = pos_get_side_to_move(pos);
    const enum colour opposing_colour = pce_swap_side(side_to_move);
    const enum square king_sq = brd_get_king_square(brd, side_to_move);

    const uint64_t occupied_sq_bb = brd_get_board_bb(brd);
    const uint64_t occ_mask = occ_mask_get_king(king_sq);
    const uint64_t opposite_colour_bb = brd_get_colour_bb(brd, opposing_colour);

    switch (gen_type) {
    case CAPTURE_ONLY:
        mv_gen_encode_multiple_capture((occ_mask & opposite_colour_bb), king_sq, mvl);
        break;
    case ALL_MOVES:
        // captures
        mv_gen_encode_multiple_capture((occ_mask & opposite_colour_bb), king_sq, mvl);
        // quiet
        mv_gen_encode_multiple_quiet((~occupied_sq_bb & occ_mask), king_sq, mvl);
        break;
    default:
        print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Invalid move gen type");
        break;
    }
}

/**
 * @brief Generates quiet moves using the given bitboard and source square.
 *
 * @param bb A bitboard repesenting all possible non-occupied squares.
 * @param from_sq The source ("from") square
 * @param mvl Pointer to a move list. All generated moves are appended to this list.
 */
static void mv_gen_encode_multiple_quiet(uint64_t bb, const enum square from_sq, struct move_list *const mvl) {
    while (bb != 0) {
        const enum square empty_sq = bb_pop_1st_bit_and_clear(&bb);
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
static void mv_gen_encode_multiple_capture(uint64_t bb, const enum square from_sq, struct move_list *const mvl) {
    while (bb != 0) {
        const enum square cap_sq = bb_pop_1st_bit_and_clear(&bb);
        const struct move cap_move = move_encode_capture(from_sq, cap_sq);
        mvl_add(mvl, cap_move);
    }
}

static void mv_gen_white_castle_moves(struct move_list *const mvl, const struct cast_perm_container cp,
                                      const uint64_t all_pce_bb) {
    if (cast_perm_has_white_kingside_permissions(cp)) {
        if ((all_pce_bb & CASTLE_SQ_MASK_WK) == 0) {
            const struct move mv = move_encode_castle_kingside_white();
            mvl_add(mvl, mv);
        }
    }
    if (cast_perm_has_white_queenside_permissions(cp)) {
        if ((all_pce_bb & CASTLE_SQ_MASK_WQ) == 0) {
            const struct move mv = move_encode_castle_queenside_white();
            mvl_add(mvl, mv);
        }
    }
}

static void mv_gen_black_castle_moves(struct move_list *const mvl, const struct cast_perm_container cp,
                                      const uint64_t all_pce_bb) {
    if (cast_perm_has_black_kingside_permissions(cp)) {
        if ((all_pce_bb & CASTLE_SQ_MASK_BK) == 0) {
            const struct move mv = move_encode_castle_kingside_black();
            mvl_add(mvl, mv);
        }
    }
    if (cast_perm_has_black_queenside_permissions(cp)) {
        if ((all_pce_bb & CASTLE_SQ_MASK_BQ) == 0) {
            const struct move mv = move_encode_castle_queenside_black();
            mvl_add(mvl, mv);
        }
    }
}

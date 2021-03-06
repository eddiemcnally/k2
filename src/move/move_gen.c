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
static const uint64_t CASTLE_MASK_WK = 0x0000000000000060;
static const uint64_t CASTLE_MASK_WQ = 0x000000000000000E;
static const uint64_t CASTLE_MASK_BK = 0x6000000000000000;
static const uint64_t CASTLE_MASK_BQ = 0x0E00000000000000;

enum move_gen_type {
    ALL_MOVES = 0,
    CAPTURE_ONLY = 1,
};

static void mv_gen_white_pawn_moves(const struct position *pos, struct move_list *mvl,
                                    const enum move_gen_type gen_type);
static void mv_gen_black_pawn_moves(const struct position *pos, struct move_list *mvl,
                                    const enum move_gen_type gen_type);
static void get_sliding_diagonal_antidiagonal_moves(const struct board *brd, const uint64_t queen_bishop_bb,
                                                    const enum colour side_to_move, struct move_list *mvl,
                                                    const enum move_gen_type gen_type);
static void get_sliding_rank_file_moves(const struct board *brd, const uint64_t rook_queen_bb,
                                        const enum colour side_to_move, struct move_list *mvl,
                                        const enum move_gen_type gen_type);
static void encode_quiet_or_capt_move(const enum square from_sq, const enum square to_sq, struct move_list *mvl,
                                      const enum move_gen_type gen_type, const uint64_t all_pce_bb);
static void try_encode_double_pawn_move(const enum square from_sq, const enum square plus_1, const enum square plus_2,
                                        struct move_list *mvl, const uint64_t all_pce_bb);
static void gen_promotions(const enum square from_sq, const enum square to_sq, struct move_list *mvl);
static void gen_promotions_with_capture(const enum square from_sq, const enum square to_sq, struct move_list *mvl);
static void mv_gen_king_knight_moves(const struct board *brd, const enum piece knight, const enum piece king,
                                     const uint64_t opposite_pce_bb, struct move_list *mvl,
                                     const enum move_gen_type gen_type);
static void mv_gen_encode_multiple_quiet(uint64_t bb, const enum square from_sq, struct move_list *mvl);
static void mv_gen_encode_multiple_capture(uint64_t bb, const enum square from_sq, struct move_list *mvl);
static void mv_gen_white_castle_moves(const struct position *pos, struct move_list *mvl);
static void mv_gen_black_castle_moves(const struct position *pos, struct move_list *mvl);
static void mv_gen_moves(const struct position *pos, struct move_list *mvl, const enum move_gen_type gen_type);

/**
 * @brief       Generates only capture moves from the given position
 * @details     Appends all new moves to the given move list
 * @param pos   The position
 * @param mvl   The move list
 */
void mv_gen_only_capture_moves(const struct position *pos, struct move_list *mvl) {

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
void mv_gen_all_moves(const struct position *pos, struct move_list *mvl) {
    assert(validate_position(pos));
    assert(validate_move_list(mvl));

    mv_gen_moves(pos, mvl, ALL_MOVES);
}

static void mv_gen_moves(const struct position *pos, struct move_list *mvl, const enum move_gen_type gen_type) {
    struct board *brd = pos_get_board(pos);
    enum colour side_to_move = pos_get_side_to_move(pos);

    switch (side_to_move) {
    case WHITE: {
        const uint64_t opposite_pce_bb = brd_get_colour_bb(brd, BLACK);
        mv_gen_king_knight_moves(brd, WHITE_KNIGHT, WHITE_KING, opposite_pce_bb, mvl, gen_type);

        // conflate bishop and queen
        const uint64_t white_queen_bishop_bb = brd_get_white_bishop_queen_bb(brd);
        get_sliding_diagonal_antidiagonal_moves(brd, white_queen_bishop_bb, WHITE, mvl, gen_type);

        const uint64_t white_rook_queen_bb = brd_get_white_rook_queen_bb(brd);
        get_sliding_rank_file_moves(brd, white_rook_queen_bb, WHITE, mvl, gen_type);

        if (gen_type == ALL_MOVES) {
            mv_gen_white_castle_moves(pos, mvl);
        }
        mv_gen_white_pawn_moves(pos, mvl, gen_type);
    } break;

    case BLACK: {
        const uint64_t opposite_pce_bb = brd_get_colour_bb(brd, WHITE);
        mv_gen_king_knight_moves(brd, BLACK_KNIGHT, BLACK_KING, opposite_pce_bb, mvl, gen_type);

        // conflate bishop and queen
        const uint64_t black_queen_bishop_bb = brd_get_black_bishop_queen_bb(brd);
        get_sliding_diagonal_antidiagonal_moves(brd, black_queen_bishop_bb, BLACK, mvl, gen_type);

        const uint64_t black_rook_queen_bb = brd_get_black_rook_queen_bb(brd);
        get_sliding_rank_file_moves(brd, black_rook_queen_bb, BLACK, mvl, gen_type);

        if (gen_type == ALL_MOVES) {
            mv_gen_black_castle_moves(pos, mvl);
        }
        mv_gen_black_pawn_moves(pos, mvl, gen_type);
    } break;

    default:
        REQUIRE(false, "Invalid side");
    }
}

/**
 * @brief Generate White Pawn moves
 * 
 * @param pos The position
 * @param mvl The move list
 * @param gen_type The type of moves to generate
 */
static void mv_gen_white_pawn_moves(const struct position *pos, struct move_list *mvl,
                                    const enum move_gen_type gen_type) {
    assert(validate_position(pos));
    assert(validate_move_list(mvl));

    const struct board *brd = pos_get_board(pos);

    uint64_t all_pawns_bb = brd_get_piece_bb(brd, WHITE_PAWN);
    const uint64_t all_pce_bb = brd_get_board_bb(brd);
    const uint64_t black_pce_bb = brd_get_colour_bb(brd, BLACK);

    const enum square en_pass_sq = pos_get_en_pass_sq(pos);
    uint64_t en_pass_sq_as_bb = 0;
    if (en_pass_sq != NO_SQUARE) {
        en_pass_sq_as_bb = bb_get_square_as_bb(en_pass_sq);
    }

    while (all_pawns_bb != 0) {
        const enum square from_sq = bb_pop_1st_bit(all_pawns_bb);
        bb_clear_square(&all_pawns_bb, from_sq);

        const enum rank pawn_rank = sq_get_rank(from_sq);

        switch (pawn_rank) {
        case RANK_2: {
            if (gen_type == ALL_MOVES) {
                // double first move
                const enum square from_plus_1 = sq_get_square_plus_1_rank(from_sq);
                const enum square from_plus_2 = sq_get_square_plus_1_rank(from_plus_1);
                try_encode_double_pawn_move(from_sq, from_plus_1, from_plus_2, mvl, all_pce_bb);

                if (bb_is_set(all_pce_bb, from_plus_1) == false) {
                    struct move quiet_move = move_encode_quiet(from_sq, from_plus_1);
                    mvl_add(mvl, quiet_move);
                }
            }

            // capture moves
            const uint64_t occ_mask = occ_mask_get_white_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = black_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square capt_to_sq = bb_pop_1st_bit(capt_bb);
                bb_clear_square(&capt_bb, capt_to_sq);
                struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
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
                const enum square prom_capt_to_sq = bb_pop_1st_bit(capt_bb);
                bb_clear_square(&capt_bb, prom_capt_to_sq);
                gen_promotions_with_capture(from_sq, prom_capt_to_sq, mvl);
            }
        } break;

        default: {
            // quiet moves
            if (gen_type == ALL_MOVES) {
                const enum square to_sq = sq_get_square_plus_1_rank(from_sq);
                if (bb_is_set(all_pce_bb, to_sq) == false) {
                    struct move quiet_move = move_encode_quiet(from_sq, to_sq);
                    mvl_add(mvl, quiet_move);
                }
            }

            // capture moves
            const uint64_t occ_mask = occ_mask_get_white_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = black_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square capt_to_sq = bb_pop_1st_bit(capt_bb);
                bb_clear_square(&capt_bb, capt_to_sq);
                struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
                mvl_add(mvl, capt_move);
            }
            // en passant
            if (en_pass_sq != NO_SQUARE) {
                // check to see if current square is attacking the en passant square
                const uint64_t pawn_attack_bb = occ_mask_get_white_pawn_capture_non_first_double_move(from_sq);
                if ((pawn_attack_bb & en_pass_sq_as_bb) != 0) {
                    const struct move en_pass_move = move_encode_enpassant(from_sq, en_pass_sq);
                    mvl_add(mvl, en_pass_move);
                }
            }

        } break;
        } //switch
    }
}

/**
 * @brief Generate Black Pawn moves
 * 
 * @param pos The position
 * @param mvl The move list
 * @param gen_type The type of moves to generate
 */
static void mv_gen_black_pawn_moves(const struct position *pos, struct move_list *mvl,
                                    const enum move_gen_type gen_type) {
    assert(validate_position(pos));
    assert(validate_move_list(mvl));

    const struct board *brd = pos_get_board(pos);

    uint64_t all_pawns_bb = brd_get_piece_bb(brd, BLACK_PAWN);
    const uint64_t all_pce_bb = brd_get_board_bb(brd);
    const uint64_t white_pce_bb = brd_get_colour_bb(brd, WHITE);

    const enum square en_pass_sq = pos_get_en_pass_sq(pos);
    uint64_t en_pass_sq_as_bb = 0;
    if (en_pass_sq != NO_SQUARE) {
        en_pass_sq_as_bb = bb_get_square_as_bb(en_pass_sq);
    }

    while (all_pawns_bb != 0) {
        const enum square from_sq = bb_pop_1st_bit(all_pawns_bb);
        bb_clear_square(&all_pawns_bb, from_sq);

        const enum rank pawn_rank = sq_get_rank(from_sq);

        switch (pawn_rank) {
        case RANK_7: {
            if (gen_type == ALL_MOVES) {
                // double first move
                const enum square from_minus_1 = sq_get_square_minus_1_rank(from_sq);
                const enum square from_minus_2 = sq_get_square_minus_1_rank(from_minus_1);
                try_encode_double_pawn_move(from_sq, from_minus_1, from_minus_2, mvl, all_pce_bb);

                if (bb_is_set(all_pce_bb, from_minus_1) == false) {
                    struct move quiet_move = move_encode_quiet(from_sq, from_minus_1);
                    mvl_add(mvl, quiet_move);
                }
            }

            // capture moves
            const uint64_t occ_mask = occ_mask_get_black_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = white_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square capt_to_sq = bb_pop_1st_bit(capt_bb);
                bb_clear_square(&capt_bb, capt_to_sq);
                struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
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
                const enum square prom_capt_to_sq = bb_pop_1st_bit(capt_bb);
                bb_clear_square(&capt_bb, prom_capt_to_sq);
                gen_promotions_with_capture(from_sq, prom_capt_to_sq, mvl);
            }
        } break;

        default: {
            // quiet moves
            if (gen_type == ALL_MOVES) {
                const enum square to_sq = sq_get_square_minus_1_rank(from_sq);
                if (bb_is_set(all_pce_bb, to_sq) == false) {
                    struct move quiet_move = move_encode_quiet(from_sq, to_sq);
                    mvl_add(mvl, quiet_move);
                }
            }

            // capture moves
            const uint64_t occ_mask = occ_mask_get_black_pawn_capture_non_first_double_move(from_sq);
            uint64_t capt_bb = white_pce_bb & occ_mask;
            while (capt_bb != 0) {
                const enum square capt_to_sq = bb_pop_1st_bit(capt_bb);
                bb_clear_square(&capt_bb, capt_to_sq);
                struct move capt_move = move_encode_capture(from_sq, capt_to_sq);
                mvl_add(mvl, capt_move);
            }
            // en passant
            if (en_pass_sq != NO_SQUARE) {
                // check to see if current square is attacking the en passant square
                const uint64_t pawn_attack_bb = occ_mask_get_black_pawn_capture_non_first_double_move(from_sq);
                if ((pawn_attack_bb & en_pass_sq_as_bb) != 0) {
                    const struct move en_pass_move = move_encode_enpassant(from_sq, en_pass_sq);
                    mvl_add(mvl, en_pass_move);
                }
            }

        } break;
        } //switch
    }
}

/**
 * @brief Generate sliding diagonal and antidiagonal moves 
 * 
 * @param brd The board
 * @param queen_bishop_bb   A bitboard representing the conflated queen and bishop pieces 
 * @param side_to_move The side to move
 * @param mvl The move list
 * @param gen_type The move type to generate
 */
static void get_sliding_diagonal_antidiagonal_moves(const struct board *brd, const uint64_t queen_bishop_bb,
                                                    const enum colour side_to_move, struct move_list *mvl,
                                                    const enum move_gen_type gen_type) {

    assert((gen_type == CAPTURE_ONLY) || (gen_type == ALL_MOVES));
    assert(validate_colour(side_to_move));

    const uint64_t side_to_move_pce_bb =
        side_to_move == WHITE ? brd_get_colour_bb(brd, WHITE) : brd_get_colour_bb(brd, BLACK);
    const uint64_t all_pce_bb = brd_get_board_bb(brd);

    uint64_t pce_to_move_bb = queen_bishop_bb;

    while (pce_to_move_bb != 0) {

        const enum square from_sq = bb_pop_1st_bit(pce_to_move_bb);
        bb_clear_square(&pce_to_move_bb, from_sq);

        const struct diagonals diag_masks = occ_mask_get_diagonals(from_sq);

        // create slider bb for this square
        const uint64_t bb_slider = bb_get_square_as_bb(from_sq);

        // diagonal move
        uint64_t diag1 = (all_pce_bb & diag_masks.positive) - (2 * bb_slider);
        uint64_t diag2 = bb_reverse(bb_reverse(all_pce_bb & diag_masks.positive) - (2 * bb_reverse(bb_slider)));
        const uint64_t diagpos = diag1 ^ diag2;

        // anti-diagonal moves
        diag1 = (all_pce_bb & diag_masks.negative) - (2 * bb_slider);
        diag2 = bb_reverse(bb_reverse(all_pce_bb & diag_masks.negative) - (2 * bb_reverse(bb_slider)));
        const uint64_t diagneg = diag1 ^ diag2;

        const uint64_t all_moves = (diagpos & diag_masks.positive) | (diagneg & diag_masks.negative);

        // get all same colour as piece being considered
        uint64_t excl_same_col = all_moves & ~side_to_move_pce_bb;

        while (excl_same_col != 0) {
            enum square to_sq = bb_pop_1st_bit(excl_same_col);
            bb_clear_square(&excl_same_col, to_sq);

            encode_quiet_or_capt_move(from_sq, to_sq, mvl, gen_type, all_pce_bb);
        }
    }
}

/**
 * @brief Generate all sliding moves for rank and files
 * 
 * @param brd The board
 * @param rook_queen_bb The conflated bitboard for queen and rook pieces 
 * @param side_to_move The side to move
 * @param mvl The move List
 * @param gen_type The type of moves to generate
 */
static void get_sliding_rank_file_moves(const struct board *brd, const uint64_t rook_queen_bb,
                                        const enum colour side_to_move, struct move_list *mvl,
                                        const enum move_gen_type gen_type) {

    // taken from https://www.chessprogramming.org/Hyperbola_Quintessence

    const uint64_t all_pce_bb = brd_get_board_bb(brd);
    const uint64_t side_to_move_pce_bb =
        side_to_move == WHITE ? brd_get_colour_bb(brd, WHITE) : brd_get_colour_bb(brd, BLACK);

    uint64_t pce_to_move_bb = rook_queen_bb;

    while (pce_to_move_bb != 0) {

        const enum square from_sq = bb_pop_1st_bit(pce_to_move_bb);
        bb_clear_square(&pce_to_move_bb, from_sq);

        const uint64_t hmask = occ_mask_get_horizontal(from_sq);
        const uint64_t vmask = occ_mask_get_vertical(from_sq);

        // create slider bb for this square
        const uint64_t bb_slider = bb_get_square_as_bb(from_sq);

        const uint64_t horiz1 = all_pce_bb - (2 * bb_slider);
        const uint64_t horiz2 = bb_reverse(bb_reverse(all_pce_bb) - (2 * bb_reverse(bb_slider)));
        const uint64_t horizontal = horiz1 ^ horiz2;

        const uint64_t vert1 = (all_pce_bb & vmask) - (2 * bb_slider);
        const uint64_t vert2 = bb_reverse(bb_reverse(all_pce_bb & vmask) - (2 * bb_reverse(bb_slider)));
        const uint64_t vertical = vert1 ^ vert2;

        const uint64_t all_moves = (horizontal & hmask) | (vertical & vmask);

        uint64_t excl_same_col = all_moves & ~side_to_move_pce_bb;
        while (excl_same_col != 0) {
            enum square to_sq = bb_pop_1st_bit(excl_same_col);
            bb_clear_square(&excl_same_col, to_sq);
            encode_quiet_or_capt_move(from_sq, to_sq, mvl, gen_type, all_pce_bb);
        }
    }
}

static void encode_quiet_or_capt_move(const enum square from_sq, const enum square to_sq, struct move_list *mvl,
                                      const enum move_gen_type gen_type, const uint64_t all_pce_bb) {

    assert((gen_type == CAPTURE_ONLY) || (gen_type == ALL_MOVES));

    struct move mv;
    switch (gen_type) {
    case CAPTURE_ONLY:
        if (bb_is_set(all_pce_bb, to_sq)) {
            mv = move_encode_capture(from_sq, to_sq);
            mvl_add(mvl, mv);
        }
        break;
    case ALL_MOVES:
        if (bb_is_set(all_pce_bb, to_sq)) {
            mv = move_encode_capture(from_sq, to_sq);
        } else {
            mv = move_encode_quiet(from_sq, to_sq);
        }
        mvl_add(mvl, mv);
        break;
    }
}

static void try_encode_double_pawn_move(const enum square from_sq, const enum square plus_1, const enum square plus_2,
                                        struct move_list *mvl, const uint64_t all_pce_bb) {
    if (!bb_is_set(all_pce_bb, plus_1) && !bb_is_set(all_pce_bb, plus_2)) {
        struct move quiet_move = move_encode_pawn_double_first(from_sq, plus_2);
        mvl_add(mvl, quiet_move);
    }
}

static void gen_promotions(const enum square from_sq, const enum square to_sq, struct move_list *mvl) {

    struct move mv = move_encode_promote_knight(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_bishop(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_rook(from_sq, to_sq);
    mvl_add(mvl, mv);

    mv = move_encode_promote_queen(from_sq, to_sq);
    mvl_add(mvl, mv);
}

static void gen_promotions_with_capture(const enum square from_sq, const enum square to_sq, struct move_list *mvl) {

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
 * @brief Generates Knight and King moves
 * 
 * @param brd       the board
 * @param knight    the knight piece
 * @param king      the king piece
 * @param opposite_pce_bb the opposite colour pieces as a bitboard
 * @param mvl       the move list
 * @param gen_type  the move types to generate
 */
static void mv_gen_king_knight_moves(const struct board *brd, const enum piece knight, const enum piece king,
                                     const uint64_t opposite_pce_bb, struct move_list *mvl,
                                     const enum move_gen_type gen_type) {

    assert((gen_type == CAPTURE_ONLY) || (gen_type == ALL_MOVES));

    const uint64_t all_pce_bb = brd_get_board_bb(brd);
    const uint64_t free_squares = ~all_pce_bb;

    // knight
    uint64_t knight_bb = brd_get_piece_bb(brd, knight);
    while (knight_bb != 0) {
        const enum square from_sq = bb_pop_1st_bit(knight_bb);
        bb_clear_square(&knight_bb, from_sq);

        const uint64_t occ_mask = occ_mask_get_knight(from_sq);

        switch (gen_type) {
        case CAPTURE_ONLY:
            mv_gen_encode_multiple_capture((occ_mask & opposite_pce_bb), from_sq, mvl);
            break;
        case ALL_MOVES:
            // captures
            mv_gen_encode_multiple_capture((occ_mask & opposite_pce_bb), from_sq, mvl);
            // quiet moves
            mv_gen_encode_multiple_quiet((free_squares & occ_mask), from_sq, mvl);
            break;
        }
    }

    // king
    uint64_t king_bb = brd_get_piece_bb(brd, king);
    const enum square from_sq = bb_pop_1st_bit(king_bb);
    const uint64_t occ_mask = occ_mask_get_king(from_sq);
    switch (gen_type) {
    case CAPTURE_ONLY:
        mv_gen_encode_multiple_capture((occ_mask & opposite_pce_bb), from_sq, mvl);
        break;
    case ALL_MOVES:
        // captures
        mv_gen_encode_multiple_capture((occ_mask & opposite_pce_bb), from_sq, mvl);
        // quiet
        mv_gen_encode_multiple_quiet((free_squares & occ_mask), from_sq, mvl);
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
static void mv_gen_encode_multiple_quiet(uint64_t bb, const enum square from_sq, struct move_list *mvl) {
    while (bb != 0) {
        const enum square empty_sq = bb_pop_1st_bit(bb);
        bb_clear_square(&bb, empty_sq);

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
static void mv_gen_encode_multiple_capture(uint64_t bb, const enum square from_sq, struct move_list *mvl) {
    while (bb != 0) {
        const enum square cap_sq = bb_pop_1st_bit(bb);
        bb_clear_square(&bb, cap_sq);
        const struct move cap_move = move_encode_capture(from_sq, cap_sq);
        mvl_add(mvl, cap_move);
    }
}

static void mv_gen_white_castle_moves(const struct position *pos, struct move_list *mvl) {
    const struct cast_perm_container cp = pos_get_cast_perm(pos);
    const struct board *brd = pos_get_board(pos);
    const uint64_t occupied_bb = brd_get_board_bb(brd);

    if (cast_perm_has_white_kingside_permissions(cp)) {
        if ((occupied_bb & CASTLE_MASK_WK) == 0) {
            const struct move mv = move_encode_castle_kingside_white();
            mvl_add(mvl, mv);
        }
    }
    if (cast_perm_has_white_queenside_permissions(cp)) {
        if ((occupied_bb & CASTLE_MASK_WQ) == 0) {
            const struct move mv = move_encode_castle_queenside_white();
            mvl_add(mvl, mv);
        }
    }
}

static void mv_gen_black_castle_moves(const struct position *pos, struct move_list *mvl) {
    const struct cast_perm_container cp = pos_get_cast_perm(pos);
    const struct board *brd = pos_get_board(pos);
    const uint64_t occupied_bb = brd_get_board_bb(brd);

    if (cast_perm_has_black_kingside_permissions(cp)) {
        if ((occupied_bb & CASTLE_MASK_BK) == 0) {
            const struct move mv = move_encode_castle_kingside_black();
            mvl_add(mvl, mv);
        }
    }
    if (cast_perm_has_black_queenside_permissions(cp)) {
        if ((occupied_bb & CASTLE_MASK_BQ) == 0) {
            const struct move mv = move_encode_castle_queenside_black();
            mvl_add(mvl, mv);
        }
    }
}

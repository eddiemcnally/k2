/*  MIT License
 *
 *  Copyright (c) 2020 Eddie McNally
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

/*! @addtogroup Position
 *
 * @ingroup Attack_Checker
 * @{
 * @details Code to test the legality of a move
 *
 */

#include "attack_checker.h"
#include "bitboard.h"
#include "board.h"
#include "occupancy_mask.h"
#include "square.h"
#include "stats.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

static bool is_white_attacking(const struct position *const pos, const enum square sq);
static bool is_black_attacking(const struct position *const pos, const enum square sq);
static bool is_horizontal_or_vertical_attacking(const uint64_t all_pce_bb, const uint64_t attacking_pce_bb,
                                                const enum square sq);
static bool is_diagonally_attacked(const uint64_t all_pce_bb, const uint64_t attacking_pce_bb, const enum square sq);
static bool is_white_pawn_attacking(const uint64_t pawn_bb, const enum square sq);
static bool is_black_pawn_attacking(const uint64_t pawn_bb, const enum square sq);
static bool is_knight_attacking(const uint64_t knight_bb, const enum square sq);
static bool is_king_attacking(const enum square king_sq, const enum square sq);

static void register_attack_type(const struct position *const pos, const enum attacking_type att);

bool att_chk_is_sq_attacked(const struct position *const pos, const enum square sq, const enum colour attacking_side) {

    assert(validate_position(pos));
    assert(validate_square(sq));
    assert(validate_colour(attacking_side));

    switch (attacking_side) {
    case WHITE:
        return is_white_attacking(pos, sq);
    case BLACK:
        return is_black_attacking(pos, sq);
    }
}

inline static bool is_white_attacking(const struct position *const pos, const enum square sq) {
    const struct board *brd = pos_get_board(pos);
    const uint64_t all_pce_bb = brd_get_board_bb(brd);

    // conflate rook and queen
    const uint64_t rook_queen_bb = brd_get_white_rook_queen_bb(brd);
    if (rook_queen_bb != 0) {
        if (is_horizontal_or_vertical_attacking(all_pce_bb, rook_queen_bb, sq)) {
            register_attack_type(pos, ATT_VERT_HORIZ);
            return true;
        }
    }

    // conflate bishop and queen
    const uint64_t bishop_queen_bb = brd_get_white_bishop_queen_bb(brd);
    if (bishop_queen_bb) {
        if (is_diagonally_attacked(all_pce_bb, bishop_queen_bb, sq)) {
            register_attack_type(pos, ATT_DIAGONAL);
            return true;
        }
    }

    const uint64_t knight_bb = brd_get_piece_bb(brd, WHITE_KNIGHT);
    if (knight_bb != 0) {
        if (is_knight_attacking(knight_bb, sq)) {
            register_attack_type(pos, ATT_KNIGHT);
            return true;
        }
    }

    const uint64_t wp_bb = brd_get_piece_bb(brd, WHITE_PAWN);
    if (wp_bb != 0) {
        if (is_white_pawn_attacking(wp_bb, sq)) {
            register_attack_type(pos, ATT_PAWN);
            return true;
        }
    }

    const enum square king_sq = brd_get_white_king_square(brd);
    if (is_king_attacking(king_sq, sq)) {
        register_attack_type(pos, ATT_KING);
        return true;
    }
    return false;
}

inline static bool is_black_attacking(const struct position *const pos, const enum square sq) {
    const struct board *brd = pos_get_board(pos);
    const uint64_t all_pce_bb = brd_get_board_bb(brd);

    // conflate rook and queen
    const uint64_t rook_queen_bb = brd_get_black_rook_queen_bb(brd);
    if (rook_queen_bb != 0) {
        if (is_horizontal_or_vertical_attacking(all_pce_bb, rook_queen_bb, sq)) {
            register_attack_type(pos, ATT_VERT_HORIZ);
            return true;
        }
    }

    // conflate bishop and queen
    const uint64_t bishop_queen_bb = brd_get_black_bishop_queen_bb(brd);
    if (bishop_queen_bb != 0) {
        if (is_diagonally_attacked(all_pce_bb, bishop_queen_bb, sq)) {
            register_attack_type(pos, ATT_DIAGONAL);
            return true;
        }
    }

    const uint64_t knight_bb = brd_get_piece_bb(brd, BLACK_KNIGHT);
    if (knight_bb != 0) {
        if (is_knight_attacking(knight_bb, sq)) {
            register_attack_type(pos, ATT_KNIGHT);
            return true;
        }
    }

    const uint64_t bp_bb = brd_get_piece_bb(brd, BLACK_PAWN);
    if (bp_bb != 0) {
        if (is_black_pawn_attacking(bp_bb, sq)) {
            register_attack_type(pos, ATT_PAWN);
            return true;
        }
    }

    const enum square king_sq = brd_get_black_king_square(brd);
    if (is_king_attacking(king_sq, sq)) {
        register_attack_type(pos, ATT_KING);
        return true;
    }

    return false;
}

inline static bool is_white_pawn_attacking(const uint64_t pawn_bb, const enum square sq) {
    const uint64_t attacking_bb = occ_mask_get_bb_white_pawns_attacking_sq(sq);
    return (attacking_bb & pawn_bb) != 0;
}

inline static bool is_black_pawn_attacking(const uint64_t pawn_bb, const enum square sq) {
    const uint64_t attacking_bb = occ_mask_get_bb_black_pawns_attacking_sq(sq);
    return (attacking_bb & pawn_bb) != 0;
}

static bool is_knight_attacking(const uint64_t knight_bb, const enum square sq) {
    uint64_t bb = knight_bb;

    // conflate all knight attack squares for all knights
    uint64_t knight_attack_sq = 0;
    while (bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit(bb);
        bb_clear_square(&bb, pce_sq);
        knight_attack_sq |= occ_mask_get_knight(pce_sq);
    }

    return bb_is_set(knight_attack_sq, sq);
}

static bool is_king_attacking(const enum square king_sq, const enum square sq) {
    const uint64_t occ_mask = occ_mask_get_king(king_sq);
    return bb_is_set(occ_mask, sq);
}

static bool is_horizontal_or_vertical_attacking(const uint64_t all_pce_bb, const uint64_t attacking_pce_bb,
                                                const enum square sq) {
    const enum rank sq_rank = sq_get_rank(sq);
    const enum file sq_file = sq_get_file(sq);

    uint64_t bb = attacking_pce_bb;

    while (bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit(bb);
        bb_clear_square(&bb, pce_sq);

        if ((sq_get_rank(pce_sq) == sq_rank) || (sq_get_file(pce_sq) == sq_file)) {
            // possible attack, shared rank and/or file
            const uint64_t inter_sq = occ_mask_get_inbetween(pce_sq, sq);
            if ((inter_sq & all_pce_bb) == 0) {
                // no blocking pieces, is attacked
                return true;
            }
        }
    }
    return false;
}

static bool is_diagonally_attacked(const uint64_t all_pce_bb, const uint64_t attacking_pce_bb, const enum square sq) {
    uint64_t bb = attacking_pce_bb;

    while (bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit(bb);
        bb_clear_square(&bb, pce_sq);
        const uint64_t occ_mask_bishop = occ_mask_get_bishop(pce_sq);

        if (bb_is_set(occ_mask_bishop, sq)) {
            // the square is potentially attacked (on same diagonal/anti-diagonal)
            const uint64_t intervening_sq_bb = occ_mask_get_inbetween(pce_sq, sq);
            if ((intervening_sq_bb & all_pce_bb) == 0) {
                // no blocking pieces, so square is attacked
                return true;
            }
        }
    }
    return false;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
__attribute__((always_inline)) static void register_attack_type(const struct position *const pos,
                                                                const enum attacking_type att) {

#ifdef ENABLE_STATS
    stats_reg_illegal_move_attacking_pce(pos_get_stats(pos), att);
#endif
}
#pragma GCC diagnostic pop

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
#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

static bool is_white_attacking(const struct position *const pos, const enum square sq);
static bool is_black_attacking(const struct position *const pos, const enum square sq);
static bool is_horizontal_or_vertical_attacking(const uint64_t all_pce_bb, const uint64_t attacking_pce_bb,
                                                const enum square sq);
static bool is_diagonally_attacked(const uint64_t all_pce_bb, const uint64_t attacking_pce_bb, const enum square sq);
static bool is_knight_attacking(const uint64_t knight_bb, const enum square sq);

/**
 * @brief Tests if a the given square is being attacked by the given colour
 * @details For a given position, tests if the given square is attacked by any piece of the given colour.
 * 
 * @param pos The position
 * @param sq The square to test
 * @param attacking_side The attacking colour
 * @return True if attacked, false otherwise
 */
bool att_chk_is_sq_attacked(const struct position *const pos, const enum square sq, const enum colour attacking_side) {

    assert(validate_position(pos));
    assert(validate_square(sq));
    assert(validate_colour(attacking_side));

    switch (attacking_side) {
    case WHITE:
        return is_white_attacking(pos, sq);
    case BLACK:
        return is_black_attacking(pos, sq);
    default:
        REQUIRE(false, "Invalid attacking side");
        break;
    }
}

static bool is_white_attacking(const struct position *const pos, const enum square sq) {
    const struct board *const brd = pos_get_board(pos);
    const uint64_t all_pce_bb = brd_get_board_bb(brd);

    // conflate rook and queen
    const uint64_t rook_queen_bb = brd_get_rook_queen_bb_for_colour(brd, WHITE);
    if (is_horizontal_or_vertical_attacking(all_pce_bb, rook_queen_bb, sq)) {
        return true;
    }

    // conflate bishop and queen
    const uint64_t bishop_queen_bb = brd_get_bishop_queen_bb_for_colour(brd, WHITE);
    if (is_diagonally_attacked(all_pce_bb, bishop_queen_bb, sq)) {
        return true;
    }

    const uint64_t knight_bb = brd_get_piece_bb(brd, WHITE_KNIGHT);
    if (is_knight_attacking(knight_bb, sq)) {
        return true;
    }

    const uint64_t wp_bb = brd_get_piece_bb(brd, WHITE_PAWN);
    const uint64_t attacking_bb = occ_mask_get_bb_white_pawns_attacking_sq(sq);
    if ((attacking_bb & wp_bb) != 0) {
        return true;
    }

    const enum square king_sq = brd_get_king_square(brd, WHITE);
    const uint64_t occ_mask = occ_mask_get_king(king_sq);
    if (bb_is_set(occ_mask, sq)) {
        return true;
    }

    return false;
}

static bool is_black_attacking(const struct position *const pos, const enum square sq) {
    const struct board *const brd = pos_get_board(pos);
    const uint64_t all_pce_bb = brd_get_board_bb(brd);

    // conflate rook and queen
    const uint64_t rook_queen_bb = brd_get_rook_queen_bb_for_colour(brd, BLACK);
    if (is_horizontal_or_vertical_attacking(all_pce_bb, rook_queen_bb, sq)) {
        return true;
    }

    // conflate bishop and queen
    const uint64_t bishop_queen_bb = brd_get_bishop_queen_bb_for_colour(brd, BLACK);
    if (is_diagonally_attacked(all_pce_bb, bishop_queen_bb, sq)) {
        return true;
    }

    const uint64_t knight_bb = brd_get_piece_bb(brd, BLACK_KNIGHT);
    if (is_knight_attacking(knight_bb, sq)) {
        return true;
    }

    const uint64_t bp_bb = brd_get_piece_bb(brd, BLACK_PAWN);
    const uint64_t attacking_bb = occ_mask_get_bb_black_pawns_attacking_sq(sq);
    if ((attacking_bb & bp_bb) != 0) {
        return true;
    }

    const enum square king_sq = brd_get_king_square(brd, BLACK);
    const uint64_t occ_mask = occ_mask_get_king(king_sq);
    if (bb_is_set(occ_mask, sq)) {
        return true;
    }

    return false;
}

static bool is_knight_attacking(const uint64_t knight_bb, const enum square sq) {
    uint64_t bb = knight_bb;

    // conflate all knight attack squares for all knights
    uint64_t knight_attack_sq = 0;
    while (bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit_and_clear(&bb);
        knight_attack_sq |= occ_mask_get_knight(pce_sq);
    }

    return bb_is_set(knight_attack_sq, sq);
}

static bool is_horizontal_or_vertical_attacking(const uint64_t all_pce_bb, const uint64_t attacking_pce_bb,
                                                const enum square sq) {
    // do a quick check before evaluating each piece
    const uint64_t sq_horiz_vert_bb = occ_mask_get_vertical(sq) | occ_mask_get_horizontal(sq);
    if ((sq_horiz_vert_bb & attacking_pce_bb) == 0) {
        // nothing shares rank or file with square
        return false;
    }

    uint64_t bb = attacking_pce_bb & sq_horiz_vert_bb;
    while (bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit_and_clear(&bb);
        const uint64_t inter_sq = occ_mask_get_inbetween(pce_sq, sq);
        if ((inter_sq & all_pce_bb) == 0) {
            // no blocking pieces, is attacked
            return true;
        }
    }
    return false;
}

static bool is_diagonally_attacked(const uint64_t all_pce_bb, const uint64_t attacking_pce_bb, const enum square sq) {

    // do a quick check to see if the attacking pieces share a diagonal or anti-diagonal with
    // the square, before checking each individual attacking piece
    const struct diagonals diags = occ_mask_get_diagonals(sq);
    const uint64_t diag_bb = diags.positive | diags.negative;
    if ((diag_bb & attacking_pce_bb) == false) {
        // nothing shares a diagonal with this square
        return false;
    }

    // only need to check pieces that share diags with square
    uint64_t bb = attacking_pce_bb & diag_bb;
    while (bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit_and_clear(&bb);
        const uint64_t intervening_sq_bb = occ_mask_get_inbetween(pce_sq, sq);
        if ((intervening_sq_bb & all_pce_bb) == 0) {
            // no blocking pieces, so square is attacked
            return true;
        }
    }
    return false;
}

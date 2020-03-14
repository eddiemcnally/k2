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

#include "attack_checker.h"
#include "bitboard.h"
#include "board.h"
#include "occupancy_mask.h"
#include "square.h"

#include <stdbool.h>
#include <stdint.h>

static bool is_white_pawn_attacking(const struct board *brd,
                                    const enum square sq);
static bool is_black_pawn_attacking(const struct board *brd,
                                    const enum square sq);
static bool is_knight_attacking(const struct board *brd, const enum square sq,
                                const struct piece attacking_pce);
static bool is_pawn_attacking(const uint64_t pawn_bb, const enum rank r,
                              const enum file f);
static bool is_king_attacking(const struct board *brd, const enum square sq,
                              const struct piece attacking_pce);
static bool is_horizontal_or_vertical_attacking(const uint64_t all_pce_bb,
                                                uint64_t attacking_pce_bb,
                                                const enum square sq);
static uint64_t in_between(const enum square sq1, const enum square sq2);
static bool is_diagonally_attacked(const uint64_t all_pce_bb,
                                   uint64_t attacking_pce_bb,
                                   const enum square sq);

bool att_chk_is_sq_attacked(const struct board *brd, const enum square sq,
                            const enum colour attacking_side) {
    const uint64_t all_pce_bb = brd_get_board_bb(brd);

    if (attacking_side == WHITE) {
        if (is_white_pawn_attacking(brd, sq)) {
            return true;
        }

        if (is_knight_attacking(brd, sq, WHITE_KNIGHT)) {
            return true;
        }

        if (is_king_attacking(brd, sq, WHITE_KING)) {
            return true;
        }

        // conflate rook and queen
        uint64_t pce_bb = brd_get_piece_bb(brd, ROOK, WHITE);
        pce_bb |= brd_get_piece_bb(brd, QUEEN, WHITE);
        if (is_horizontal_or_vertical_attacking(all_pce_bb, pce_bb, sq)) {
            return true;
        }

        // conflate bishop and queen
        pce_bb = brd_get_piece_bb(brd, BISHOP, WHITE);
        pce_bb |= brd_get_piece_bb(brd, QUEEN, WHITE);
        if (is_diagonally_attacked(all_pce_bb, pce_bb, sq)) {
            return true;
        }

    } else {
        if (is_black_pawn_attacking(brd, sq)) {
            return true;
        }
        if (is_knight_attacking(brd, sq, BLACK_KNIGHT)) {
            return true;
        }
        if (is_king_attacking(brd, sq, BLACK_KING)) {
            return true;
        }

        // conflate rook and queen
        uint64_t pce_bb = brd_get_piece_bb(brd, ROOK, BLACK);
        pce_bb |= brd_get_piece_bb(brd, QUEEN, BLACK);
        if (is_horizontal_or_vertical_attacking(all_pce_bb, pce_bb, sq)) {
            return true;
        }

        // conflate bishop and queen
        pce_bb = brd_get_piece_bb(brd, BISHOP, BLACK);
        pce_bb |= brd_get_piece_bb(brd, QUEEN, BLACK);
        if (is_diagonally_attacked(all_pce_bb, pce_bb, sq)) {
            return true;
        }
    }

    return false;
}

static bool is_white_pawn_attacking(const struct board *brd,
                                    const enum square sq) {

    const enum rank sq_rank = sq_get_rank(sq);
    const enum file sq_file = sq_get_file(sq);
    const uint64_t wp_pce_bb = brd_get_piece_bb(brd, PAWN, WHITE);

    if (wp_pce_bb == 0) {
        return false;
    }

    // check down and left
    if (is_pawn_attacking(wp_pce_bb, sq_rank - 1, sq_file - 1)) {
        return true;
    }

    // check down and right
    if (is_pawn_attacking(wp_pce_bb, sq_rank - 1, sq_file + 1)) {
        return true;
    }

    return false;
}

static bool is_black_pawn_attacking(const struct board *brd,
                                    const enum square sq) {

    const enum rank sq_rank = sq_get_rank(sq);
    const enum file sq_file = sq_get_file(sq);
    const uint64_t bp_pce_bb = brd_get_piece_bb(brd, PAWN, BLACK);

    if (bp_pce_bb == 0) {
        return false;
    }

    // check up and left
    if (is_pawn_attacking(bp_pce_bb, sq_rank + 1, sq_file - 1)) {
        return true;
    }

    // check up and right
    if (is_pawn_attacking(bp_pce_bb, sq_rank + 1, sq_file + 1)) {
        return true;
    }

    return false;
}

static bool is_pawn_attacking(const uint64_t pawn_bb, const enum rank r,
                              const enum file f) {
    enum square pawn_sq;
    const bool is_valid = sq_try_get_sq(r, f, &pawn_sq);
    if (is_valid) {
        return bb_is_set(pawn_bb, pawn_sq);
    }
    return false;
}

static bool is_knight_attacking(const struct board *brd, const enum square sq,
                                const struct piece attacking_pce) {
    const enum colour attacking_col = pce_get_colour(attacking_pce);

    uint64_t pce_bb = brd_get_piece_bb(brd, KNIGHT, attacking_col);

    while (pce_bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit(&pce_bb);
        const uint64_t occ_mask = occ_mask_get_knight(pce_sq);
        if (bb_is_set(occ_mask, sq)) {
            return true;
        }
    }
    return false;
}

static bool is_king_attacking(const struct board *brd, const enum square sq,
                              const struct piece attacking_pce) {
    const enum colour attacking_col = pce_get_colour(attacking_pce);
    uint64_t pce_bb = brd_get_piece_bb(brd, KING, attacking_col);
    const enum square pce_sq = bb_pop_1st_bit(&pce_bb);

    const uint64_t occ_mask = occ_mask_get_king(pce_sq);
    if (bb_is_set(occ_mask, sq)) {
        return true;
    }
    return false;
}

static bool is_horizontal_or_vertical_attacking(const uint64_t all_pce_bb,
                                                uint64_t attacking_pce_bb,
                                                const enum square sq) {
    const enum rank sq_rank = sq_get_rank(sq);
    const enum file sq_file = sq_get_file(sq);

    while (attacking_pce_bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit(&attacking_pce_bb);

        if ((sq_get_rank(pce_sq) == sq_rank) ||
            (sq_get_file(pce_sq) == sq_file)) {
            // possible attack
            const uint64_t inter_sq = in_between(pce_sq, sq);
            if ((inter_sq & all_pce_bb) == 0) {
                // no blocking pieces
                return true;
            }
        }
    }
    return false;
}

static bool is_diagonally_attacked(const uint64_t all_pce_bb,
                                   uint64_t attacking_pce_bb,
                                   const enum square sq) {

    while (attacking_pce_bb != 0) {
        const enum square pce_sq = bb_pop_1st_bit(&attacking_pce_bb);
        const uint64_t occ_mask_bishop = occ_mask_get_bishop(pce_sq);

        if (bb_is_set(occ_mask_bishop, sq)) {
            // the square is potentially attacked (on same diagonal/anti-diagonal)
            const uint64_t intervening_sq_bb = in_between(pce_sq, sq);
            if ((intervening_sq_bb & all_pce_bb) == 0) {
                // no blocking pieces, so square is attacked
                return true;
            }
        }
    }
    return false;
}

// This code returns a bitboard with bits set representing squares between
// the given 2 squares.
//
// The code is taken from :
// https://www.chessprogramming.org/Square_Attacked_By#LegalityTest
//
static uint64_t in_between(const enum square sq1, const enum square sq2) {
    const uint64_t m1 = 0xffffffffffffffff;
    const uint64_t a2a7 = 0x0001010101010100;
    const uint64_t b2g7 = 0x0040201008040200;
    const uint64_t h1b7 = 0x0002040810204080;
    uint64_t btwn, line, rank, file;

    btwn = (m1 << sq1) ^ (m1 << sq2);
    file = (sq2 & 7) - (sq1 & 7);
    rank = ((sq2 | 7) - sq1) >> 3;
    line = ((file & 7) - 1) & a2a7;            /* a2a7 if same file */
    line += 2 * (((rank & 7) - 1) >> 58);      /* b1g1 if same rank */
    line += (((rank - file) & 15) - 1) & b2g7; /* b2g7 if same diagonal */
    line += (((rank + file) & 15) - 1) & h1b7; /* h1b7 if same antidiag */
    line *= btwn & -btwn; /* mul acts like shift by smaller square */

    return line & btwn; /* return the bits on that line in-between */
}

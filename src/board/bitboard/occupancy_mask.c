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

/*! @addtogroup OccupancyMasks
 *
 * @ingroup OccupancyMasks
 * @{
 * @details A set of pre-calculated occupancy masks for various pieces on each square.
 *
 */

#include "occupancy_mask.h"
#include "bitboard.h"
#include "piece.h"
#include "square.h"
#include <assert.h>

static uint64_t in_between(const enum square sq1, const enum square sq2);
static void gen_in_between_sq_mask(void);
static void gen_knight_mask(void);
static void set_dest_sq_if_valid(enum rank rank, enum file file, uint64_t *bb);
static void gen_king_mask(void);
static void occ_mask_gen_diagonal_occupancy_masks(void);
static void gen_bishop_mask(void);
static void gen_queen_mask(void);
static void gen_rook_mask(void);

#define RANK_MASK ((uint64_t)0x00000000000000ff)
#define FILE_MASK ((uint64_t)0x0101010101010101)

#define FILE_A_BB FILE_MASK
#define FILE_H_BB (FILE_A_BB << 7)

#define NORTH_EAST(bb) ((bb & ~FILE_H_BB) << 9)
#define SOUTH_EAST(bb) ((bb & ~FILE_H_BB) >> 7)
#define NORTH_WEST(bb) ((bb & ~FILE_A_BB) << 7)
#define SOUTH_WEST(bb) ((bb & ~FILE_A_BB) >> 9)

//
//
// NOTES :
//              - For these occupancy masks, element [0] is a1, and [63] is h8
//              - The bit mask represents the set of possible target squares for a piece on a given square
//
static uint64_t in_between_sq[NUM_SQUARES][NUM_SQUARES] = {{0}, {0}};
static uint64_t knight_occupancy_masks[NUM_SQUARES] = {0};
static uint64_t king_occupancy_masks[NUM_SQUARES] = {0};
static struct diagonals diagonal_masks[NUM_SQUARES] = {0};
static uint64_t bishop_occupancy_masks[NUM_SQUARES] = {0};
static uint64_t queen_occupancy_masks[NUM_SQUARES] = {0};
static uint64_t rook_occupancy_masks[NUM_SQUARES] = {0};

void occ_mask_init(void) {
    gen_in_between_sq_mask();
    gen_knight_mask();
    gen_king_mask();
    occ_mask_gen_diagonal_occupancy_masks();
    gen_bishop_mask();

    gen_rook_mask();
    gen_queen_mask();
}

uint64_t occ_mask_get_inbetween(const enum square sq1, const enum square sq2) {
    assert(validate_square(sq1));
    assert(validate_square(sq2));
    return in_between_sq[sq1][sq2];
}

struct diagonals occ_mask_get_diagonals(const enum square sq) {
    assert(validate_square(sq));
    return diagonal_masks[sq];
}

inline uint64_t occ_mask_get_vertical(const enum square sq) {
    assert(validate_square(sq));

    const enum file f = sq_get_file(sq);
    return FILE_MASK << f;
}

inline uint64_t occ_mask_get_horizontal(const enum square sq) {
    assert(validate_square(sq));

    const enum rank r = sq_get_rank(sq);
    return RANK_MASK << (r << 3); // rank * 8
}

/**
 * @brief Returns a bitboard representing WHITE pawns that can attack the given square
 * 
 * @param sq The square being attacked
 * @return uint64_t A bitboard representing WHITE pawns that can attack the square
 */
inline uint64_t occ_mask_get_bb_white_pawns_attacking_sq(const enum square sq) {
    const uint64_t bb = bb_get_square_as_bb(sq);
    return SOUTH_EAST(bb) | SOUTH_WEST(bb);
}

/**
 * @brief Returns a bitboard representing BLACK pawns that can attack the given square
 * 
 * @param sq The square being attacked
 * @return uint64_t A bitboard representing BLACK pawns that can attack the square
 */
inline uint64_t occ_mask_get_bb_black_pawns_attacking_sq(const enum square sq) {
    const uint64_t bb = bb_get_square_as_bb(sq);
    return NORTH_EAST(bb) | NORTH_WEST(bb);
}

/**
 * @brief               Get white pawn capture non-first move occupancy mask
 *
 * @param sq    The square containing the pawn
 * @return A bitboard representing the occupancy mask
 */
inline uint64_t occ_mask_get_white_pawn_capture_non_first_double_move(const enum square sq) {
    assert(validate_square(sq));

    const uint64_t sq_bb = bb_get_square_as_bb(sq);
    return NORTH_EAST(sq_bb) | NORTH_WEST(sq_bb);
}

/**
 * @brief               Get black pawn capture non-first move occupancy mask
 *
 * @param sq    The square containing the pawn
 * @return A bitboard representing the occupancy mask
 */
inline uint64_t occ_mask_get_black_pawn_capture_non_first_double_move(const enum square sq) {
    assert(validate_square(sq));

    const uint64_t sq_bb = bb_get_square_as_bb(sq);
    return SOUTH_EAST(sq_bb) | SOUTH_WEST(sq_bb);
}

/**
 * @brief               Get Knight occupancy mask for the given square
 *
 * @param sq    The square containing the knight
 * @return A bitboard representing the occupancy mask
 */
uint64_t occ_mask_get_knight(const enum square sq) {
    assert(validate_square(sq));
    return knight_occupancy_masks[sq];
}

/**
 * @brief               Get Bishop occupancy mask for the given square
 *
 * @param sq    The square containing the Bishop
 * @return A bitboard representing the occupancy mask
 */
uint64_t occ_mask_get_bishop(const enum square sq) {
    assert(validate_square(sq));
    return bishop_occupancy_masks[sq];
}

/**
 * @brief               Get King occupancy mask for the given square
 *
 * @param sq    The square containing the King
 * @return A bitboard representing the occupancy mask
 */
uint64_t occ_mask_get_king(const enum square sq) {
    assert(validate_square(sq));
    return king_occupancy_masks[sq];
}

/**
 * @brief               Get Queen occupancy mask for the given square
 *
 * @param sq    The square containing the Queen
 * @return A bitboard representing the occupancy mask
 */
uint64_t occ_mask_get_queen(const enum square sq) {
    assert(validate_square(sq));
    return queen_occupancy_masks[sq];
}

/**
 * @brief               Get Rook occupancy mask for the given square
 *
 * @param sq    The square containing the Rook
 * @return A bitboard representing the occupancy mask
 */
uint64_t occ_mask_get_rook(const enum square sq) {
    assert(validate_square(sq));
    return rook_occupancy_masks[sq];
}

static void gen_in_between_sq_mask(void) {
    for (enum square i = a1; i <= h8; i++) {
        for (enum square j = a1; j <= h8; j++) {
            in_between_sq[i][j] = in_between(i, j);
        }
    }
}

// The code is taken from :
// https://www.chessprogramming.org/Square_Attacked_By#LegalityTest
//
static uint64_t in_between(const enum square sq1, const enum square sq2) {
#define M1 0xffffffffffffffff
#define A2A7 0x0001010101010100
#define B2G7 0x0040201008040200
#define H1B7 0x0002040810204080

    const uint64_t btwn = (M1 << sq1) ^ (M1 << sq2);
    const uint64_t file = (sq2 & 7) - (sq1 & 7);
    const uint64_t rank = ((sq2 | 7) - sq1) >> 3;
    uint64_t line = ((file & 7) - 1) & A2A7;   /* a2a7 if same file */
    line += 2 * (((rank & 7) - 1) >> 58);      /* b1g1 if same rank */
    line += (((rank - file) & 15) - 1) & B2G7; /* b2g7 if same diagonal */
    line += (((rank + file) & 15) - 1) & H1B7; /* h1b7 if same antidiag */
    line *= btwn & -btwn;                      /* mul acts like shift by smaller square */

    return line & btwn; /* return the bits on that line in-between */
}

static void gen_queen_mask(void) {
    for (enum square sq = a1; sq <= h8; sq++) {
        const uint64_t bishop_mask = bishop_occupancy_masks[sq];
        const uint64_t rook_mask = occ_mask_get_rook(sq);

        uint64_t queen = bishop_mask | rook_mask;
        // clear our square
        bb_clear_square(&queen, sq);
        queen_occupancy_masks[sq] = queen;
    }
}

static void gen_rook_mask(void) {
    for (enum square sq = a1; sq <= h8; sq++) {
        const uint64_t horiz_mask = occ_mask_get_horizontal(sq);
        const uint64_t vert_mask = occ_mask_get_vertical(sq);

        rook_occupancy_masks[sq] = (horiz_mask | vert_mask);
    }
}

static void gen_knight_mask(void) {

    for (enum square sq = a1; sq <= h8; sq++) {
        enum rank dest_rank = 0;
        enum file dest_file = 0;

        // 8 destination squares are:
        // (-2 +8), (-1 + 16), (+1 + 16) (+2 + 8),
        // (-2 -8), (-1 - 16), (+1 - 16) (+2 - 8),

        enum rank rank = sq_get_rank(sq);
        enum file file = sq_get_file(sq);

        uint64_t b = 0;

        // left 1 file, up 2 ranks
        dest_rank = rank + 2;
        dest_file = file - 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // left 1 file, down 2 ranks
        dest_rank = rank - 2;
        dest_file = file - 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // left 2 files, up 1 rank
        dest_rank = rank + 1;
        dest_file = file - 2;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // left 2 files, down 1 rank
        dest_rank = rank - 1;
        dest_file = file - 2;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // right 1 file, up 2 ranks
        dest_rank = rank + 2;
        dest_file = file + 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // right 1 file, down 2 ranks
        dest_rank = rank - 2;
        dest_file = file + 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // right 2 files, up 1 rank
        dest_rank = rank + 1;
        dest_file = file + 2;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // right 2 files, down 1 rank
        dest_rank = rank - 1;
        dest_file = file + 2;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        knight_occupancy_masks[sq] = b;
    }
}

static void gen_king_mask(void) {
    for (enum square sq = a1; sq <= h8; sq++) {
        // valid king moves, and resulting distance vector
        // +7, +8, +9
        // -1,  K, +1
        // -9, -8, -7

        // the equates to ranks and files as follows (rank/file):
        //      (+1, -1),       (+1, 0),        (+1, +1)
        //       (0, -1),       XXX,            (0, +1)
        // etc
        enum rank dest_rank = 0;
        enum file dest_file = 0;

        enum rank rank = sq_get_rank(sq);
        enum file file = sq_get_file(sq);

        uint64_t b = 0;

        // left 1 file, up 1 rank
        dest_rank = rank + 1;
        dest_file = file - 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // no file move, up 1 rank
        dest_rank = rank + 1;
        dest_file = file;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // right 1 file, up 1 rank
        dest_rank = rank + 1;
        dest_file = file + 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // left 1 file, no rank change
        dest_rank = rank;
        dest_file = file - 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // right 1 file, no rank change
        dest_rank = rank;
        dest_file = file + 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // right 1 file, down 1 rank
        dest_rank = rank - 1;
        dest_file = file + 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // no file move, down 1 rank
        dest_rank = rank - 1;
        dest_file = file;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        // left 1 file, down 1 rank
        dest_rank = rank - 1;
        dest_file = file - 1;
        set_dest_sq_if_valid(dest_rank, dest_file, &b);

        king_occupancy_masks[sq] = b;
    }
}

static void occ_mask_gen_diagonal_occupancy_masks(void) {
    for (enum square sq = a1; sq <= h8; sq++) {

        enum rank rank = sq_get_rank(sq);
        enum file file = sq_get_file(sq);

        enum rank dest_rank = 0;
        enum file dest_file = 0;
        uint64_t b = 0;

        // move SW
        dest_rank = rank;
        dest_file = file;
        while (sq_is_valid_file(dest_file) && sq_is_valid_rank(dest_rank)) {
            set_dest_sq_if_valid(dest_rank, dest_file, &b);
            dest_rank--;
            dest_file--;
        }
        // clear our square
        bb_clear_square(&b, sq);
        diagonal_masks[sq].positive |= b;

        // move NW
        b = 0;
        dest_rank = rank;
        dest_file = file;
        while (sq_is_valid_file(dest_file) && sq_is_valid_rank(dest_rank)) {
            set_dest_sq_if_valid(dest_rank, dest_file, &b);
            dest_rank++;
            dest_file--;
        }
        // clear our square
        bb_clear_square(&b, sq);
        diagonal_masks[sq].negative |= b;

        // move SE
        b = 0;
        dest_rank = rank;
        dest_file = file;
        while (sq_is_valid_file(dest_file) && sq_is_valid_rank(dest_rank)) {
            set_dest_sq_if_valid(dest_rank, dest_file, &b);
            dest_rank--;
            dest_file++;
        }
        // clear our square
        bb_clear_square(&b, sq);
        diagonal_masks[sq].negative |= b;

        // move NE
        b = 0;
        dest_rank = rank;
        dest_file = file;
        while (sq_is_valid_file(dest_file) && sq_is_valid_rank(dest_rank)) {
            set_dest_sq_if_valid(dest_rank, dest_file, &b);
            dest_rank++;
            dest_file++;
        }
        // clear our square
        bb_clear_square(&b, sq);
        diagonal_masks[sq].positive |= b;
    }
}

static void gen_bishop_mask(void) {

    for (enum square sq = a1; sq <= h8; sq++) {

        enum rank rank = sq_get_rank(sq);
        enum file file = sq_get_file(sq);

        //printf("rank/file : %d/%d\n", rank, file);

        enum rank dest_rank = 0;
        enum file dest_file = 0;
        uint64_t b = 0;

        // move left and down
        dest_rank = rank;
        dest_file = file;
        while (sq_is_valid_file(dest_file) && sq_is_valid_rank(dest_rank)) {
            set_dest_sq_if_valid(dest_rank, dest_file, &b);
            dest_rank--;
            dest_file--;
        }

        // move left and up
        dest_rank = rank;
        dest_file = file;
        while (sq_is_valid_file(dest_file) && sq_is_valid_rank(dest_rank)) {
            set_dest_sq_if_valid(dest_rank, dest_file, &b);
            dest_rank++;
            dest_file--;
        }

        // move right and down
        dest_rank = rank;
        dest_file = file;
        while (sq_is_valid_file(dest_file) && sq_is_valid_rank(dest_rank)) {
            set_dest_sq_if_valid(dest_rank, dest_file, &b);
            dest_rank--;
            dest_file++;
        }

        // move right and up
        dest_rank = rank;
        dest_file = file;
        while (sq_is_valid_file(dest_file) && sq_is_valid_rank(dest_rank)) {
            set_dest_sq_if_valid(dest_rank, dest_file, &b);
            dest_rank++;
            dest_file++;
        }

        // clear our square
        bb_clear_square(&b, sq);

        bishop_occupancy_masks[sq] = b;
    }
}

static void set_dest_sq_if_valid(enum rank rank, enum file file, uint64_t *bb) {
    if (sq_is_valid_file(file) && sq_is_valid_rank(rank)) {
        enum square dest_sq = sq_gen_from_rank_file(rank, file);
        bb_set_square(bb, (enum square)dest_sq);
        //printf("---- OK  rank/file (sq=%d): %d/%d\n", dest_sq, rank, file);
    } else {
        //printf("XXXX bad rank/file: %d/%d\n", rank, file);
    }
}

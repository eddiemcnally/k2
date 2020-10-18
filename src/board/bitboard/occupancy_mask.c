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

#define RANK_MASK ((uint64_t)0x00000000000000ff)
#define FILE_MASK ((uint64_t)0x0101010101010101)

#define FILE_A_BB FILE_MASK
// #define FILE_B_BB   (FILE_A_BB << 1)
// #define FILE_C_BB   (FILE_A_BB << 2)
// #define FILE_D_BB   (FILE_A_BB << 3)
// #define FILE_E_BB   (FILE_A_BB << 4)
// #define FILE_F_BB   (FILE_A_BB << 5)
// #define FILE_G_BB   (FILE_A_BB << 6)
#define FILE_H_BB (FILE_A_BB << 7)

// #define NORTH(bb)   (bb << 8)
// #define SOUTH(bb)   (bb >> 8)
// #define EAST(bb)    ((bb & ~FILE_H_BB) << 1)
// #define WEST(bb)    ((bb & ~FILE_A_BB) >> 1)

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
static const uint64_t knight_occupancy_masks[NUM_SQUARES] = {
    0x0000000000020400, 0x0000000000050800, 0x00000000000a1100, 0x0000000000142200, 0x0000000000284400,
    0x0000000000508800, 0x0000000000a01000, 0x0000000000402000, 0x0000000002040004, 0x0000000005080008,
    0x000000000a110011, 0x0000000014220022, 0x0000000028440044, 0x0000000050880088, 0x00000000a0100010,
    0x0000000040200020, 0x0000000204000402, 0x0000000508000805, 0x0000000a1100110a, 0x0000001422002214,
    0x0000002844004428, 0x0000005088008850, 0x000000a0100010a0, 0x0000004020002040, 0x0000020400040200,
    0x0000050800080500, 0x00000a1100110a00, 0x0000142200221400, 0x0000284400442800, 0x0000508800885000,
    0x0000a0100010a000, 0x0000402000204000, 0x0002040004020000, 0x0005080008050000, 0x000a1100110a0000,
    0x0014220022140000, 0x0028440044280000, 0x0050880088500000, 0x00a0100010a00000, 0x0040200020400000,
    0x0204000402000000, 0x0508000805000000, 0x0a1100110a000000, 0x1422002214000000, 0x2844004428000000,
    0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000, 0x0400040200000000, 0x0800080500000000,
    0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000,
    0x2000204000000000, 0x0004020000000000, 0x0008050000000000, 0x00110a0000000000, 0x0022140000000000,
    0x0044280000000000, 0x0088500000000000, 0x0010a00000000000, 0x0020400000000000};

static const uint64_t bishop_occupancy_masks[NUM_SQUARES] = {
    0x8040201008040200, 0x0080402010080500, 0x0000804020110a00, 0x0000008041221400, 0x0000000182442800,
    0x0000010204885000, 0x000102040810a000, 0x0102040810204000, 0x4020100804020002, 0x8040201008050005,
    0x00804020110a000a, 0x0000804122140014, 0x0000018244280028, 0x0001020488500050, 0x0102040810a000a0,
    0x0204081020400040, 0x2010080402000204, 0x4020100805000508, 0x804020110a000a11, 0x0080412214001422,
    0x0001824428002844, 0x0102048850005088, 0x02040810a000a010, 0x0408102040004020, 0x1008040200020408,
    0x2010080500050810, 0x4020110a000a1120, 0x8041221400142241, 0x0182442800284482, 0x0204885000508804,
    0x040810a000a01008, 0x0810204000402010, 0x0804020002040810, 0x1008050005081020, 0x20110a000a112040,
    0x4122140014224180, 0x8244280028448201, 0x0488500050880402, 0x0810a000a0100804, 0x1020400040201008,
    0x0402000204081020, 0x0805000508102040, 0x110a000a11204080, 0x2214001422418000, 0x4428002844820100,
    0x8850005088040201, 0x10a000a010080402, 0x2040004020100804, 0x0200020408102040, 0x0500050810204080,
    0x0a000a1120408000, 0x1400142241800000, 0x2800284482010000, 0x5000508804020100, 0xa000a01008040201,
    0x4000402010080402, 0x0002040810204080, 0x0005081020408000, 0x000a112040800000, 0x0014224180000000,
    0x0028448201000000, 0x0050880402010000, 0x00a0100804020100, 0x0040201008040201};

static const uint64_t queen_occupancy_masks[NUM_SQUARES] = {
    0x81412111090503fe, 0x02824222120a07fd, 0x0404844424150efb, 0x08080888492a1cf7, 0x10101011925438ef,
    0x2020212224a870df, 0x404142444850e0bf, 0x8182848890a0c07f, 0x412111090503fe03, 0x824222120a07fd07,
    0x04844424150efb0e, 0x080888492a1cf71c, 0x101011925438ef38, 0x20212224a870df70, 0x4142444850e0bfe0,
    0x82848890a0c07fc0, 0x2111090503fe0305, 0x4222120a07fd070a, 0x844424150efb0e15, 0x0888492a1cf71c2a,
    0x1011925438ef3854, 0x212224a870df70a8, 0x42444850e0bfe050, 0x848890a0c07fc0a0, 0x11090503fe030509,
    0x22120a07fd070a12, 0x4424150efb0e1524, 0x88492a1cf71c2a49, 0x11925438ef385492, 0x2224a870df70a824,
    0x444850e0bfe05048, 0x8890a0c07fc0a090, 0x090503fe03050911, 0x120a07fd070a1222, 0x24150efb0e152444,
    0x492a1cf71c2a4988, 0x925438ef38549211, 0x24a870df70a82422, 0x4850e0bfe0504844, 0x90a0c07fc0a09088,
    0x0503fe0305091121, 0x0a07fd070a122242, 0x150efb0e15244484, 0x2a1cf71c2a498808, 0x5438ef3854921110,
    0xa870df70a8242221, 0x50e0bfe050484442, 0xa0c07fc0a0908884, 0x03fe030509112141, 0x07fd070a12224282,
    0x0efb0e1524448404, 0x1cf71c2a49880808, 0x38ef385492111010, 0x70df70a824222120, 0xe0bfe05048444241,
    0xc07fc0a090888482, 0xfe03050911214181, 0xfd070a1222428202, 0xfb0e152444840404, 0xf71c2a4988080808,
    0xef38549211101010, 0xdf70a82422212020, 0xbfe0504844424140, 0x7fc0a09088848281};

static const uint64_t king_occupancy_masks[NUM_SQUARES] = {
    0x0000000000000302, 0x0000000000000705, 0x0000000000000e0a, 0x0000000000001c14, 0x0000000000003828,
    0x0000000000007050, 0x000000000000e0a0, 0x000000000000c040, 0x0000000000030203, 0x0000000000070507,
    0x00000000000e0a0e, 0x00000000001c141c, 0x0000000000382838, 0x0000000000705070, 0x0000000000e0a0e0,
    0x0000000000c040c0, 0x0000000003020300, 0x0000000007050700, 0x000000000e0a0e00, 0x000000001c141c00,
    0x0000000038283800, 0x0000000070507000, 0x00000000e0a0e000, 0x00000000c040c000, 0x0000000302030000,
    0x0000000705070000, 0x0000000e0a0e0000, 0x0000001c141c0000, 0x0000003828380000, 0x0000007050700000,
    0x000000e0a0e00000, 0x000000c040c00000, 0x0000030203000000, 0x0000070507000000, 0x00000e0a0e000000,
    0x00001c141c000000, 0x0000382838000000, 0x0000705070000000, 0x0000e0a0e0000000, 0x0000c040c0000000,
    0x0003020300000000, 0x0007050700000000, 0x000e0a0e00000000, 0x001c141c00000000, 0x0038283800000000,
    0x0070507000000000, 0x00e0a0e000000000, 0x00c040c000000000, 0x0302030000000000, 0x0705070000000000,
    0x0e0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000,
    0xc040c00000000000, 0x0203000000000000, 0x0507000000000000, 0x0a0e000000000000, 0x141c000000000000,
    0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000};

/* indexed using enum square
 * Represents the bottom-left to top-right diagonals that a bishop can move to, when
 * on a specific square
 */
static const uint64_t positive_diagonal_masks[] = {
    0x8040201008040200, 0x0080402010080400, 0x0000804020100800, 0x0000008040201000, 0x0000000080402000,
    0x0000000000804000, 0x0000000000008000, 0x0000000000000000, 0x4020100804020000, 0x8040201008040001,
    0x0080402010080002, 0x0000804020100004, 0x0000008040200008, 0x0000000080400010, 0x0000000000800020,
    0x0000000000000040, 0x2010080402000000, 0x4020100804000100, 0x8040201008000201, 0x0080402010000402,
    0x0000804020000804, 0x0000008040001008, 0x0000000080002010, 0x0000000000004020, 0x1008040200000000,
    0x2010080400010000, 0x4020100800020100, 0x8040201000040201, 0x0080402000080402, 0x0000804000100804,
    0x0000008000201008, 0x0000000000402010, 0x0804020000000000, 0x1008040001000000, 0x2010080002010000,
    0x4020100004020100, 0x8040200008040201, 0x0080400010080402, 0x0000800020100804, 0x0000000040201008,
    0x0402000000000000, 0x0804000100000000, 0x1008000201000000, 0x2010000402010000, 0x4020000804020100,
    0x8040001008040201, 0x0080002010080402, 0x0000004020100804, 0x0200000000000000, 0x0400010000000000,
    0x0800020100000000, 0x1000040201000000, 0x2000080402010000, 0x4000100804020100, 0x8000201008040201,
    0x0000402010080402, 0x0000000000000000, 0x0001000000000000, 0x0002010000000000, 0x0004020100000000,
    0x0008040201000000, 0x0010080402010000, 0x0020100804020100, 0x0040201008040201};

/* indexed using enum square
 * Represents the top-left to bottom-right diagonals that a bishop can move to, when
 * on a specific square
 */
static const uint64_t negative_diagonal_masks[] = {
    0x0000000000000000, 0x0000000000000100, 0x0000000000010200, 0x0000000001020400, 0x0000000102040800,
    0x0000010204081000, 0x0001020408102000, 0x0102040810204000, 0x0000000000000002, 0x0000000000010004,
    0x0000000001020008, 0x0000000102040010, 0x0000010204080020, 0x0001020408100040, 0x0102040810200080,
    0x0204081020400000, 0x0000000000000204, 0x0000000001000408, 0x0000000102000810, 0x0000010204001020,
    0x0001020408002040, 0x0102040810004080, 0x0204081020008000, 0x0408102040000000, 0x0000000000020408,
    0x0000000100040810, 0x0000010200081020, 0x0001020400102040, 0x0102040800204080, 0x0204081000408000,
    0x0408102000800000, 0x0810204000000000, 0x0000000002040810, 0x0000010004081020, 0x0001020008102040,
    0x0102040010204080, 0x0204080020408000, 0x0408100040800000, 0x0810200080000000, 0x1020400000000000,
    0x0000000204081020, 0x0001000408102040, 0x0102000810204080, 0x0204001020408000, 0x0408002040800000,
    0x0810004080000000, 0x1020008000000000, 0x2040000000000000, 0x0000020408102040, 0x0100040810204080,
    0x0200081020408000, 0x0400102040800000, 0x0800204080000000, 0x1000408000000000, 0x2000800000000000,
    0x4000000000000000, 0x0002040810204080, 0x0004081020408000, 0x0008102040800000, 0x0010204080000000,
    0x0020408000000000, 0x0040800000000000, 0x0080000000000000, 0x0000000000000000};

uint64_t occ_mask_get_positive_diagonal(const enum square sq) {
    assert(validate_square(sq));
    return positive_diagonal_masks[sq];
}

uint64_t occ_mask_get_negative_diagonal(const enum square sq) {
    assert(validate_square(sq));
    return negative_diagonal_masks[sq];
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
uint64_t occ_mask_get_bb_white_pawns_attacking_sq(const enum square sq) {
    const uint64_t bb = bb_get_sq_mask(sq);
    return SOUTH_EAST(bb) | SOUTH_WEST(bb);
}

/**
 * @brief Returns a bitboard representing BLACK pawns that can attack the given square
 * 
 * @param sq The square being attacked
 * @return uint64_t A bitboard representing BLACK pawns that can attack the square
 */
uint64_t occ_mask_get_bb_black_pawns_attacking_sq(const enum square sq) {
    const uint64_t bb = bb_get_sq_mask(sq);
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

    const uint64_t sq_bb = bb_set_square(0, sq);
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

    const uint64_t sq_bb = bb_set_square(0, sq);
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
inline uint64_t occ_mask_get_rook(const enum square sq) {
    assert(validate_square(sq));

    const uint64_t horiz_mask = occ_mask_get_horizontal(sq);
    const uint64_t vert_mask = occ_mask_get_vertical(sq);
    return (horiz_mask | vert_mask);
}

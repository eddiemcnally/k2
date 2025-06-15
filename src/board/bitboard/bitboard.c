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

/*! @addtogroup Bitboard
 *
 * @ingroup Bitboard
 * @{
 * @details Utility functions for manipulating uint64_t bitboards.
 *
 */

#include "bitboard.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>

static const uint64_t EMPTY_BITBOARD = 0;

/**
 * @brief       Set bit in bitboard representing the given square
 *
 * @param bb    Pointer to bitboard
 * @param sq    The square
 */
void bb_set_square(uint64_t *restrict const bb, enum square sq) {
    assert(validate_square(sq));
    // should be clear
    assert(((*bb >> sq) & 0x01) == 0);

    *bb |= ((uint64_t)0x01 << sq);
}

void bb_set_square_multi(uint64_t *restrict const bb1, uint64_t *restrict const bb2, enum square sq) {
    assert(validate_square(sq));
    // should be clear
    assert(((*bb1 >> sq) & 0x01) == 0);
    assert(((*bb2 >> sq) & 0x01) == 0);

    const uint64_t sq_as_bb = ((uint64_t)0x01 << sq);

    *bb1 |= sq_as_bb;
    *bb2 |= sq_as_bb;
}

/**
 * @brief       Clear bit in bitboard representing the given square
 *
 * @param bb    Pointer to bitboard
 * @param sq    The square
 */
void bb_clear_square(uint64_t *restrict const bb, enum square sq) {
    assert(validate_square(sq));
    assert(((*bb >> sq) & 0x01) == 1);

    *bb &= (~((uint64_t)0x01 << sq));
}

void bb_clear_square_multi(uint64_t *restrict const bb1, uint64_t *restrict const bb2, enum square sq) {
    assert(validate_square(sq));
    assert(((*bb1 >> sq) & 0x01) == 1);
    assert(((*bb2 >> sq) & 0x01) == 1);

    const uint64_t sq_as_bb = ~((uint64_t)0x01 << sq);

    *bb1 &= sq_as_bb;
    *bb2 &= sq_as_bb;
}

/**
 * @brief      Clears the bit representing the from_sq and sets the bit representing the to_sq
 *
 * @param      bb       Pointer to the bitboard
 * @param[in]  from_sq  The from sq
 * @param[in]  to_sq    To sq
 */
__attribute__((always_inline)) void bb_move_bit(uint64_t *restrict const bb, enum square from_sq, enum square to_sq) {

    assert(validate_square(from_sq));
    assert(validate_square(to_sq));
    assert(bb_is_clear(*bb, to_sq));

    const uint64_t from_bb = SQUARE_AS_BITBOARD(from_sq);
    const uint64_t to_bb = SQUARE_AS_BITBOARD(to_sq);

    // just xor's...assumes bits are set/cleared as expected before this runs
    *bb ^= from_bb;
    *bb ^= to_bb;
}

__attribute__((always_inline)) void bb_move_bit_multi(uint64_t *restrict const bb1, uint64_t *restrict const bb2,
                                                      enum square from_sq, enum square to_sq) {

    assert(validate_square(from_sq));
    assert(validate_square(to_sq));
    assert(bb_is_clear(*bb1, to_sq));
    assert(bb_is_clear(*bb1, to_sq));

    const uint64_t from_bb = SQUARE_AS_BITBOARD(from_sq);
    const uint64_t to_bb = SQUARE_AS_BITBOARD(to_sq);

    // just xor's...assumes bits are set/cleared as expected before this runs
    *bb1 ^= from_bb;
    *bb1 ^= to_bb;
    *bb2 ^= from_bb;
    *bb2 ^= to_bb;
}

/**
 * @brief       Test if a square in the given bitboard is set
 *
 * @param bb    The bitboard
 * @param sq    The square
 *
 * @return true if bit is set, false otherwise.
 */
bool bb_is_set(const uint64_t bb, enum square sq) {
    assert(validate_square(sq));

    const uint64_t b = bb >> sq;
    return (b & 0x01) != 0;
}

/**
 * @brief       Test if a square in the given bitboard is clear
 *
 * @param bb    The bitboard
 * @param sq    The square
 *
 * @return true if bit is clear, false otherwise.
 */
bool bb_is_clear(const uint64_t bb, enum square sq) {
    assert(validate_square(sq));

    const uint64_t b = bb >> sq;
    return (b & 0x01) == 0;
}

/**
 * @brief      Pops the LSB from the given u64, and clears it
 *
 * @param      bb    { parameter_description }
 *
 * @return     Square representing the bib popped, and the passed in value has the corresponding bit cleared.
 */
enum square bb_pop_1st_bit_and_clear(uint64_t *restrict const bb) {
    const int bit_num = __builtin_ctzll(*bb);
    enum square sq = (enum square)bit_num;
    bb_clear_square(bb, sq);
    return sq;
}

/**
 * @brief       Returns an empty bitboard
 *
 * @return      The empty bitboard
 */
uint64_t bb_get_empty(void) {
    return EMPTY_BITBOARD;
}

/**
 * @brief       Prints outs the bitboard as a chessboard with ranks and files
 *
 * @param bb    The bitboard
 */
void bb_print_as_board(const uint64_t bb) {
    for (int rank = RANK_8; rank >= RANK_1; rank--) {
        printf("%d  ", rank + 1); // enum is zero-based
        for (int file = FILE_A; file <= FILE_H; file++) {
            enum square sq = sq_gen_from_rank_file((enum rank)rank, (enum file)file);
            if (bb_is_set(bb, sq)) {
                printf("  X");
            } else {
                printf("  -");
            }
        }
        printf("\n");
    }

    printf("\n   ");
    for (enum file file = FILE_A; file <= FILE_H; file++) {
        printf("%2c ", 'a' + (int)file);
    }
    printf("\n\n");
}

/**
 * @brief       Reverses the bits in the given bitboard
 *
 * @param bb    The bitboard
 * @return      The reversed bitboard
 */
__attribute__((always_inline)) uint64_t bb_reverse(uint64_t bb) {
    // see https://clang.llvm.org/docs/LanguageExtensions.html#builtin-functions
    return __builtin_bitreverse64(bb);
}

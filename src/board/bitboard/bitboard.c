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
#include <assert.h>
#include <stdio.h>

static const uint64_t EMPTY_BITBOARD = 0;

/**
 * @brief       Set bit in bitboard representing the given square
 *
 * @param bb    The bitboard
 * @param sq    The square
 * @return      The bitboard with the square set
 */
inline uint64_t bb_set_square(const uint64_t bb, const enum square sq) {
    assert(validate_square(sq));

    return bb | ((uint64_t)0x01 << sq);
}

/**
 * @brief       Clear bit in bitboard representing the given square
 *
 * @param bb    The bitboard
 * @param sq    The square
 * @return      The bitboard with the square cleared
 */
inline uint64_t bb_clear_square(const uint64_t bb, const enum square sq) {
    assert(validate_square(sq));

    return bb & (~((uint64_t)0x01 << sq));
}

/**
 * @brief       Test if a square in the given bitboard is set
 *
 * @param bb    The bitboard
 * @param sq    The square
 *
 * @return true if bit is set, false otherwise.
 */
inline bool bb_is_set(const uint64_t bb, const enum square sq) {
    assert(validate_square(sq));

    return (bb & ((uint64_t)0x01 << sq)) != 0;
}

/**
 * @brief       Test if a square in the given bitboard is clear
 *
 * @param bb    The bitboard
 * @param sq    The square
 *
 * @return true if bit is set, false otherwise.
 */
inline bool bb_is_clear(const uint64_t bb, const enum square sq) {
    assert(validate_square(sq));

    return (bb_is_set(bb, sq) == false);
}

/**
 * @brief       Pops the lowest set bit.  Uses gcc built-in function
 * (see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)
 *
 * @param bb    The bitboard
 * @return      The zero-based bit that was set
 */
inline enum square bb_pop_1st_bit(const uint64_t bb) {
    int bit_num = __builtin_ctzll(bb);
    enum square sq = (enum square)bit_num;
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
inline uint64_t bb_reverse(uint64_t bb) {
    // see https://clang.llvm.org/docs/LanguageExtensions.html#builtin-functions
    return __builtin_bitreverse64(bb);
}

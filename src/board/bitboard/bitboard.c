/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */



#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "bitboard.h"

#define BIT_0 (0x01ull)

/**
 * @brief 		Set bit in bitboard representing the given square
 *
 * @param bb The bitboard
 * @param sq The square
 */
void bb_set_square ( bitboard_t* bb, const enum square sq )
{
        validate_square ( sq );
        *bb = *bb | ( BIT_0 << sq );
}

/**
 * @brief 		Clear bit in bitboard representing the given square
 *
 * @param bb The bitboard
 * @param sq The square
 */

void bb_clear_square ( bitboard_t* bb, const enum square sq )
{
        validate_square ( sq );
        *bb = *bb & ( ~ ( BIT_0 << sq ) );
}

/**
 * @brief 		Test if a square in the given bitboard is set
 *
 * @param bb The bitboard
 * @param sq The square
 *
 * @return true if bit is set, false otherwise.
 */
bool bb_is_set ( const bitboard_t bb, const enum square sq )
{
        validate_square ( sq );
        return ( bb & ( BIT_0 << sq ) ) != 0;
}


/**
 * @brief 		Counts the number of set bits in a bitboard. Uses a built-in GCC function
 *
 * @param bb    The bitboard
 * @return The number of set bits
 */
uint8_t bb_count_bits ( const bitboard_t bb )
{
        return ( uint8_t ) __builtin_popcountll ( bb );
}

/**
 * @brief 		Pops the lowest set bit, and clears the bit in the bitboard.  Uses gcc built-in function
 * (see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)
 *
 * @param bb    The bitboard
 * @return The zero-based bit that was set
 */
enum square bb_pop_1st_bit ( bitboard_t * bb )
{
        enum square sq = ( enum square ) __builtin_ctzll ( *bb );
        bb_clear_square ( bb, sq );
        return sq;
}


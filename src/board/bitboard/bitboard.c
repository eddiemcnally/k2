// Copyright (c) 2017 Eddie McNally
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "bitboard.h"

#define BIT_0 (0x01ull)

/**
 * @brief 		Set bit in bitboard
 * @details 	Sets the bit in the bitboard representing the given square
 *
 * @param bb The bitboard
 * @param square The square
 */
void bb_set_square(bitboard_t* bb, const enum square sq)
{
	validate_square(sq);
	*bb = *bb | (BIT_0 << sq);
}

/**
 * @brief 		Clear bit in bitboard
 * @details 	Clears the bit in the bitboard representing the given square
 *
 * @param bb The bitboard
 * @param square The square
 */

void bb_clear_square(bitboard_t* bb, const enum square sq)
{
	validate_square(sq);
	*bb = *bb & (~(BIT_0 << sq));
}

/**
 * @brief 		Test a square in the given bitboard
 * @details 	Tests if a bit (representing a square) is set
 *
 * @param bb The bitboard
 * @param square The square
 *
 * @return true if bit is set, false otherwise.
 */
bool bb_is_set(const bitboard_t bb, const enum square sq)
{
	validate_square(sq);
	return (bb & (BIT_0 << sq)) != 0;
}


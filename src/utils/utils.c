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
#include "bitboard.h"
#include "utils.h"

/**
 * @brief 		Pops least-significant bit
 * @details 	Pops the lowest set bit, and clears the bit in the bitboard.  Uses gcc built-in function
 * (see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)
 *
 * @param bb The bitboard
 * @return The zero-based bit that was set
 */
uint8_t pop_1st_bit(bitboard_t * bb)
{
	uint8_t bit = (uint8_t) __builtin_ctzll(*bb);

	// clear the bit
	*bb = *bb & (uint64_t) (~(0x01ull << bit));
	return bit;
}

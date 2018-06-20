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


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "bitboard.h"

#define         BIT_0           ( (uint64_t) 0x01ull)
#define         EMPTY_BITBOARD  ((uint64_t)0)

// A lookup table for reversing bits in a byte.
// See https://graphics.stanford.edu/%7Eseander/bithacks.html#BitReverseTable
#define R2(n)     n,     n + 2*64,     n + 1*64,     n + 3*64
#define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )

static const unsigned char BitReverseTable256[256] = {
        R6 ( 0 ), R6 ( 2 ), R6 ( 1 ), R6 ( 3 )
};


/**
 * @brief       Returns a bitboard with a single bit set representing the given square
 *
 * @param sq The square
 */
uint64_t bb_get_sq_mask ( const enum square sq )
{
        assert ( validate_square ( sq ) );

        uint64_t bb = 0;
        return bb | ( BIT_0 << sq );
}



/**
 * @brief       Set bit in bitboard representing the given square
 *
 * @param bb The bitboard
 * @param sq The square
 * @return given bitboard with sq bit set
 */
uint64_t bb_set_square ( const uint64_t bb, const enum square sq )
{
        assert ( validate_square ( sq ) );

        return bb | ( BIT_0 << sq );
}

/**
 * @brief       Clear bit in bitboard representing the given square
 *
 * @param bb    The bitboard
 * @param sq    The square
 * @return      The given bitboard with given square bit cleared
 */
uint64_t bb_clear_square ( const uint64_t bb, const enum square sq )
{
        assert ( validate_square ( sq ) );

        return bb & ( ~ ( BIT_0 << sq ) );
}

/**
 * @brief       Test if a square in the given bitboard is set
 *
 * @param bb    The bitboard
 * @param sq    The square
 *
 * @return true if bit is set, false otherwise.
 */
bool bb_is_set ( const uint64_t bb, const enum square sq )
{
        assert ( validate_square ( sq ) );

        return ( bb & ( BIT_0 << sq ) ) != 0;
}

/**
 * @brief       Test if a square in the given bitboard is clear
 *
 * @param bb    The bitboard
 * @param sq    The square
 *
 * @return true if bit is set, false otherwise.
 */
bool bb_is_clear ( const uint64_t bb, const enum square sq )
{
        assert ( validate_square ( sq ) );

        return (bb_is_set(bb, sq) == false);
}



/**
 * @brief               Counts the number of set bits in a bitboard. Uses a built-in GCC function
 *
 * @param bb    The bitboard
 * @return The number of set bits
 */
uint8_t bb_count_bits ( const uint64_t bb )
{
        return ( uint8_t ) __builtin_popcountll ( bb );
}

/**
 * @brief               Pops the lowest set bit, and clears the bit in the bitboard.  Uses gcc built-in function
 * (see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html)
 *
 * @param bb    The bitboard
 * @return The zero-based bit that was set
 */
enum square bb_pop_1st_bit ( uint64_t * bb )
{
        enum square sq = ( enum square ) __builtin_ctzll ( *bb );
        *bb = bb_clear_square ( *bb, sq );
        return sq;
}


uint64_t bb_get_empty ( void )
{
        return EMPTY_BITBOARD;
}

/**
 * @brief               Prints outs the bitboard as a chessboard with ranks and files
 *
 * @param bb    The bitboard
 */
void bb_print_as_board ( const uint64_t bb )
{
        for ( int rank = RANK_8; rank >= RANK_1; rank-- ) {
                printf ( "%d  ", rank + 1 );    // enum is zero-based
                for ( int file = FILE_A; file <= FILE_H; file++ ) {
                        enum square sq = sq_gen_from_rank_file ( ( enum rank ) rank, ( enum file ) file );
                        if ( bb_is_set ( bb, sq ) ) {
                                printf ( "  X" );
                        } else {
                                printf ( "  -" );
                        }
                }
                printf ( "\n" );

        }

        printf ( "\n   " );
        for ( enum file file = FILE_A; file <= FILE_H; file++ ) {
                printf ( "%2c ", 'a' + ( int ) file );
        }
        printf ( "\n\n" );

}



/**
 * @brief       Reverses the bits in the given bitboard
 *
 * @param bb    The bitboard
 * @return The reversed bitboard
 */
uint64_t bb_reverse ( uint64_t bb )
{
        uint64_t retval = 0;

        uint8_t *p_in = ( uint8_t * ) & bb;
        uint8_t *p_out = ( uint8_t * ) & retval;
        // reverse the bits in each byte
        for ( int i = 0; i < 8; i++ ) {
                *p_out = ( uint8_t ) BitReverseTable256[*p_in];
                p_out++;
                p_in++;
        }

        // now reverse the bytes
        return __builtin_bswap64 ( retval );
}



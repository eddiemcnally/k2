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


#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "piece.h"
#include "test_piece.h"


void test_piece_get_piece_from_label ( void **state )
{
        assert_true ( pce_get_from_label ( 'P' ) == WPAWN );
        assert_true ( pce_get_from_label ( 'B' ) == WBISHOP );
        assert_true ( pce_get_from_label ( 'N' ) == WKNIGHT );
        assert_true ( pce_get_from_label ( 'R' ) == WROOK );
        assert_true ( pce_get_from_label ( 'Q' ) == WQUEEN );
        assert_true ( pce_get_from_label ( 'K' ) == WKING );

        assert_true ( pce_get_from_label ( 'p' ) == BPAWN );
        assert_true ( pce_get_from_label ( 'b' ) == BBISHOP );
        assert_true ( pce_get_from_label ( 'n' ) == BKNIGHT );
        assert_true ( pce_get_from_label ( 'r' ) == BROOK );
        assert_true ( pce_get_from_label ( 'q' ) == BQUEEN );
        assert_true ( pce_get_from_label ( 'k' ) == BKING );
}

void test_piece_get_piece_label ( void **state )
{
        assert_true ( pce_get_label ( WPAWN ) == 'P' );
        assert_true ( pce_get_label ( BPAWN ) == 'p' );
        assert_true ( pce_get_label ( WBISHOP ) == 'B' );
        assert_true ( pce_get_label ( BBISHOP ) == 'b' );
        assert_true ( pce_get_label ( WKNIGHT ) == 'N' );
        assert_true ( pce_get_label ( BKNIGHT ) == 'n' );
        assert_true ( pce_get_label ( WROOK ) == 'R' );
        assert_true ( pce_get_label ( BROOK ) == 'r' );
        assert_true ( pce_get_label ( WQUEEN ) == 'Q' );
        assert_true ( pce_get_label ( BQUEEN ) == 'q' );
        assert_true ( pce_get_label ( WKING ) == 'K' );
        assert_true ( pce_get_label ( BKING ) == 'k' );
}

void test_piece_values ( void **state )
{
        assert_true ( pce_get_value ( WPAWN ) == 100 );
        assert_true ( pce_get_value ( BPAWN ) == 100 );
        assert_true ( pce_get_value ( WPAWN ) == pce_get_value ( BPAWN ) );

        assert_true ( pce_get_value ( WBISHOP ) == 325 );
        assert_true ( pce_get_value ( BBISHOP ) == 325 );
        assert_true ( pce_get_value ( WBISHOP ) == pce_get_value ( BBISHOP ) );

        assert_true ( pce_get_value ( WKNIGHT ) == 325 );
        assert_true ( pce_get_value ( BKNIGHT ) == 325 );
        assert_true ( pce_get_value ( WKNIGHT ) == pce_get_value ( BKNIGHT ) );

        assert_true ( pce_get_value ( WROOK ) == 500 );
        assert_true ( pce_get_value ( BROOK ) == 500 );
        assert_true ( pce_get_value ( WROOK ) == pce_get_value ( BROOK ) );

        assert_true ( pce_get_value ( WQUEEN ) == 1000 );
        assert_true ( pce_get_value ( BQUEEN ) == 1000 );
        assert_true ( pce_get_value ( WQUEEN ) == pce_get_value ( BQUEEN ) );

        assert_true ( pce_get_value ( WKING ) == 50000 );
        assert_true ( pce_get_value ( BKING ) == 50000 );
        assert_true ( pce_get_value ( WKING ) == pce_get_value ( BKING ) );
}

void test_piece_get_colour_white_pieces ( void **state )
{
        assert_true ( pce_get_colour ( WPAWN ) == WHITE );
        assert_true ( pce_get_colour ( WBISHOP ) == WHITE );
        assert_true ( pce_get_colour ( WKNIGHT ) == WHITE );
        assert_true ( pce_get_colour ( WROOK ) == WHITE );
        assert_true ( pce_get_colour ( WQUEEN ) == WHITE );
        assert_true ( pce_get_colour ( WKING ) == WHITE );
}

void test_piece_get_colour_black_pieces ( void **state )
{
        assert_true ( pce_get_colour ( BPAWN ) == BLACK );
        assert_true ( pce_get_colour ( BBISHOP ) == BLACK );
        assert_true ( pce_get_colour ( BKNIGHT ) == BLACK );
        assert_true ( pce_get_colour ( BROOK ) == BLACK );
        assert_true ( pce_get_colour ( BQUEEN ) == BLACK );
        assert_true ( pce_get_colour ( BKING ) == BLACK );
}

void test_piece_swap_side ( void **state )
{
        assert_true ( swap_side ( WHITE ) == BLACK );
        assert_true ( swap_side ( BLACK ) == WHITE );
}

void test_piece_is_white ( void **state )
{
        assert_true ( pce_is_white ( WPAWN ) );
        assert_true ( pce_is_white ( WBISHOP ) );
        assert_true ( pce_is_white ( WKNIGHT ) );
        assert_true ( pce_is_white ( WROOK ) );
        assert_true ( pce_is_white ( WQUEEN ) );
        assert_true ( pce_is_white ( WKING ) );
}

void test_piece_is_black ( void **state )
{
        assert_true ( pce_is_black ( BPAWN ) );
        assert_true ( pce_is_black ( BBISHOP ) );
        assert_true ( pce_is_black ( BKNIGHT ) );
        assert_true ( pce_is_black ( BROOK ) );
        assert_true ( pce_is_black ( BQUEEN ) );
        assert_true ( pce_is_black ( BKING ) );
}



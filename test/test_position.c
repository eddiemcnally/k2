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
#include "position.h"
#include "board.h"
#include "test_position.h"



void test_position_get_set_castle_permissions ( void **state )
{
        cast_perm_t cp;
        cast_perm_set_WK ( &cp, true );
        struct position *pos = pos_create();

        pos_set_cast_perm ( pos, cp );
        cast_perm_t retrieved_cp = pos_get_cast_perm ( pos );
        assert_true ( cp == retrieved_cp );

        cast_perm_set_WQ ( &cp, true );
        pos_set_cast_perm ( pos, cp );
        retrieved_cp = pos_get_cast_perm ( pos );
        assert_true ( cp == retrieved_cp );

        cast_perm_set_BK ( &cp, true );
        pos_set_cast_perm ( pos, cp );
        retrieved_cp = pos_get_cast_perm ( pos );
        assert_true ( cp == retrieved_cp );

        cast_perm_set_BQ ( &cp, true );
        pos_set_cast_perm ( pos, cp );
        retrieved_cp = pos_get_cast_perm ( pos );
        assert_true ( cp == retrieved_cp );
}


void test_position_brd_is_sq_occupied ( void **state )
{
        const char * FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/1bNnrQ2 w - - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( FEN, pos );

        struct board *brd = pos_get_board ( pos );

        assert_false ( brd_is_sq_occupied ( brd, a1 ) );
        assert_false ( brd_is_sq_occupied ( brd, a2 ) );
        assert_false ( brd_is_sq_occupied ( brd, a3 ) );
        assert_true ( brd_is_sq_occupied ( brd, a4 ) );
        assert_false ( brd_is_sq_occupied ( brd, a5 ) );
        assert_false ( brd_is_sq_occupied ( brd, a6 ) );
        assert_true ( brd_is_sq_occupied ( brd, a7 ) );
        assert_false ( brd_is_sq_occupied ( brd, a8 ) );

        assert_true ( brd_is_sq_occupied ( brd, b1 ) );
        assert_true ( brd_is_sq_occupied ( brd, b2 ) );
        assert_false ( brd_is_sq_occupied ( brd, b3 ) );
        assert_true ( brd_is_sq_occupied ( brd, b4 ) );
        assert_false ( brd_is_sq_occupied ( brd, b5 ) );
        assert_true ( brd_is_sq_occupied ( brd, b6 ) );
        assert_true ( brd_is_sq_occupied ( brd, b7 ) );
        assert_true ( brd_is_sq_occupied ( brd, b8 ) );

        assert_true ( brd_is_sq_occupied ( brd, c1 ) );
        assert_false ( brd_is_sq_occupied ( brd, c2 ) );
        assert_true ( brd_is_sq_occupied ( brd, c3 ) );
        assert_true ( brd_is_sq_occupied ( brd, c4 ) );
        assert_false ( brd_is_sq_occupied ( brd, c5 ) );
        assert_false ( brd_is_sq_occupied ( brd, c6 ) );
        assert_false ( brd_is_sq_occupied ( brd, c7 ) );
        assert_false ( brd_is_sq_occupied ( brd, c8 ) );

        assert_true ( brd_is_sq_occupied ( brd, d1 ) );
        assert_false ( brd_is_sq_occupied ( brd, d2 ) );
        assert_true ( brd_is_sq_occupied ( brd, d3 ) );
        assert_false ( brd_is_sq_occupied ( brd, d4 ) );
        assert_true ( brd_is_sq_occupied ( brd, d5 ) );
        assert_false ( brd_is_sq_occupied ( brd, d6 ) );
        assert_false ( brd_is_sq_occupied ( brd, d7 ) );
        assert_true ( brd_is_sq_occupied ( brd, d8 ) );

        assert_true ( brd_is_sq_occupied ( brd, e1 ) );
        assert_true ( brd_is_sq_occupied ( brd, e2 ) );
        assert_false ( brd_is_sq_occupied ( brd, e3 ) );
        assert_true ( brd_is_sq_occupied ( brd, e4 ) );
        assert_false ( brd_is_sq_occupied ( brd, e5 ) );
        assert_false ( brd_is_sq_occupied ( brd, e6 ) );
        assert_false ( brd_is_sq_occupied ( brd, e7 ) );
        assert_true ( brd_is_sq_occupied ( brd, e8 ) );

        assert_true ( brd_is_sq_occupied ( brd, f1 ) );
        assert_false ( brd_is_sq_occupied ( brd, f2 ) );
        assert_true ( brd_is_sq_occupied ( brd, f3 ) );
        assert_true ( brd_is_sq_occupied ( brd, f4 ) );
        assert_true ( brd_is_sq_occupied ( brd, f5 ) );
        assert_true ( brd_is_sq_occupied ( brd, f6 ) );
        assert_false ( brd_is_sq_occupied ( brd, f7 ) );
        assert_true ( brd_is_sq_occupied ( brd, f8 ) );

        assert_false ( brd_is_sq_occupied ( brd, g1 ) );
        assert_true ( brd_is_sq_occupied ( brd, g2 ) );
        assert_true ( brd_is_sq_occupied ( brd, g3 ) );
        assert_true ( brd_is_sq_occupied ( brd, g4 ) );
        assert_true ( brd_is_sq_occupied ( brd, g5 ) );
        assert_false ( brd_is_sq_occupied ( brd, g6 ) );
        assert_false ( brd_is_sq_occupied ( brd, g7 ) );
        assert_false ( brd_is_sq_occupied ( brd, g8 ) );

        assert_false ( brd_is_sq_occupied ( brd, h1 ) );
        assert_true ( brd_is_sq_occupied ( brd, h2 ) );
        assert_true ( brd_is_sq_occupied ( brd, h3 ) );
        assert_false ( brd_is_sq_occupied ( brd, h4 ) );
        assert_false ( brd_is_sq_occupied ( brd, h5 ) );
        assert_true ( brd_is_sq_occupied ( brd, h6 ) );
        assert_false ( brd_is_sq_occupied ( brd, h7 ) );
        assert_false ( brd_is_sq_occupied ( brd, h8 ) );

}






// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

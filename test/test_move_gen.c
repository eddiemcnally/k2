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
#include "move_gen.h"
#include "move_list.h"
#include "board.h"
#include "position.h"
#include "fen.h"
#include "piece.h"
#include "test_move_gen.h"

void test_move_gen_knight_white ( void **state )
{
        const char *RANDOM_FEN_1 = "R1n2b2/3p4/K1P2n2/1P2N2p/P2k1pN1/1P2p1Q1/Rpb1p3/1rB5 w - - 0 1\n";
        const uint16_t NUM_EXPECTED_WHITE_KNIGHT_MOVES = 11;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct board *brd = pos_get_board ( pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_knight_moves ( brd, WHITE, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_WHITE_KNIGHT_MOVES );

        // check quiet moves
        move_t mv = move_encode_quiet ( g4, f2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g4, h2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g4, h6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e5, c4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e5, d3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e5, f3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e5, g6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e5, f7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        // check capture moves
        mv = move_encode_capture ( g4, e3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( g4, f6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( e5, d7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
}



void test_move_gen_knight_black ( void **state )
{
        const char *RANDOM_FEN_1 = "R1n2b2/3p4/K1P2n2/1P2N2p/P2k1pN1/1P2p1Q1/Rpb1p3/1rB5 w - - 0 1\n";
        const uint16_t NUM_EXPECTED_BLACK_KNIGHT_MOVES = 10;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct board *brd = pos_get_board ( pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_knight_moves ( brd, BLACK, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_BLACK_KNIGHT_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( c8, a7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c8, b6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c8, d6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c8, e7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f6, e8 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f6, d5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f6, e4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f6, h7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f6, g8 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        // check capture moves
        mv = move_encode_capture ( f6, g4 );

}



void test_move_gen_sliding_diagonal_black ( void **state )
{
        const char *RANDOM_FEN_1 = "R1n2b2/3p4/K1P2n2/1P2N2p/P2k1pN1/1P2p1Q1/Rpb1p3/1rB5 w - - 0 1\n";
        const uint16_t NUM_EXPECTED_MOVES = 14;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct board *brd = pos_get_board ( pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_diagonal_sliding_moves( brd, BLACK, mvl );

        
        printf("***** %d\n", mvl_get_move_count ( mvl ));
        mvl_print(mvl);
        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_MOVES );

        // quiet moves
        move_t mv = move_encode_quiet ( f8, e7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f8, d6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f8, c5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f8, b4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f8, c3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f8, g7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f8, h6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c2, d3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        mv = move_encode_quiet ( c2, e4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c2, f5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c2, g6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c2, h7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c2, d1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        // capture move
        mv = move_encode_capture ( c2, b3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

}






// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

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
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>
#include "position.h"
#include "board.h"
#include "piece.h"
#include "square.h"
#include "test_board.h"

static enum piece pce_list[NUM_PIECES] = {WPAWN, WBISHOP, WKNIGHT, WROOK, WQUEEN, WKING,
                                          BPAWN, BBISHOP, BKNIGHT, BROOK, BQUEEN, BKING
                                         };

void test_board_brd_allocate_deallocate ( void **state )
{
        struct board *brd = brd_allocate();
        validate_board ( brd );
        brd_deallocate ( brd );
}


void test_board_brd_bulk_add_remove_piece ( void **state )
{
        struct board *brd = brd_allocate();

        for ( int i = 0; i < NUM_PIECES; i++ ) {
                for ( enum square sq = a1; sq <= h8; sq++ ) {
                        enum piece pce = pce_list[i];

                        // add piece
                        brd_add_piece ( brd, pce, sq );

                        // verify it's there
                        enum piece found_pce;
                        bool found = brd_try_get_piece_on_square ( brd, sq, &found_pce );
                        assert_true ( found );
                        assert_true ( found_pce == pce );
                        bool is_occupied = brd_is_sq_occupied ( brd, sq );
                        assert_true ( is_occupied );

                        // remove piece
                        brd_remove_piece ( brd, pce, sq );

                        // verify it's gone
                        found = brd_try_get_piece_on_square ( brd, sq, &found_pce );
                        assert_false ( found );
                        is_occupied = brd_is_sq_occupied ( brd, sq );
                        assert_false ( is_occupied );
                }
        }
        brd_deallocate ( brd );
}



void test_board_brd_move_piece ( void **state )
{
        struct board *brd = brd_allocate();

        for ( int i = 0; i < NUM_PIECES; i++ ) {
                enum piece pce = pce_list[i];
                for ( enum square from_sq = a1; from_sq <= h8; from_sq++ ) {
                        for ( enum square to_sq = a1; to_sq <= h8; to_sq++ ) {
                                if ( from_sq == to_sq ) {
                                        continue;
                                }
                                // add piece
                                brd_add_piece ( brd, pce, from_sq );

                                // verify it's there
                                enum piece found_pce;
                                bool found = brd_try_get_piece_on_square ( brd, from_sq, &found_pce );
                                assert_true ( found );
                                assert_true ( found_pce == pce );
                                bool is_occupied = brd_is_sq_occupied ( brd, from_sq );
                                assert_true ( is_occupied );

                                // move it
                                brd_move_piece ( brd, pce, from_sq, to_sq );

                                // verify it's not on the from_sq
                                found = brd_try_get_piece_on_square ( brd, from_sq, &found_pce );
                                assert_false ( found );
                                is_occupied = brd_is_sq_occupied ( brd, from_sq );
                                assert_false ( is_occupied );
                                // verify it's now on the to_sq
                                found = brd_try_get_piece_on_square ( brd, to_sq, &found_pce );
                                assert_true ( found );
                                is_occupied = brd_is_sq_occupied ( brd, to_sq );
                                assert_true ( is_occupied );

                                // remove piece
                                brd_remove_piece ( brd, pce, to_sq );
                        }
                }
        }

        brd_deallocate ( brd );
}


void test_board_brd_get_piece_bb ( void **state )
{
        const char * FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/1bNnrQ2 w - - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( FEN, pos );

        struct board *brd = pos_get_board ( pos );

        bitboard_t bb = brd_get_piece_bb ( brd, WPAWN );
        assert_true ( bb_is_set ( bb, b2 ) );
        assert_true ( bb_is_set ( bb, d3 ) );
        assert_true ( bb_is_set ( bb, e2 ) );
        assert_true ( bb_is_set ( bb, f5 ) );
        assert_true ( bb_is_set ( bb, g2 ) );
        assert_true ( bb_is_set ( bb, g4 ) );
        assert_true ( bb_is_set ( bb, g5 ) );
        assert_true ( bb_is_set ( bb, h2 ) );

        bb = brd_get_piece_bb ( brd, WBISHOP );
        assert_true ( bb_is_set ( bb, b7 ) );
        assert_true ( bb_is_set ( bb, f8 ) );

        bb = brd_get_piece_bb ( brd, WKNIGHT );
        assert_true ( bb_is_set ( bb, c1 ) );
        assert_true ( bb_is_set ( bb, e8 ) );

        bb = brd_get_piece_bb ( brd, WROOK );
        assert_true ( bb_is_set ( bb, a4 ) );
        assert_true ( bb_is_set ( bb, d8 ) );

        bb = brd_get_piece_bb ( brd, WQUEEN );
        assert_true ( bb_is_set ( bb, f1 ) );

        bb = brd_get_piece_bb ( brd, BPAWN );
        assert_true ( bb_is_set ( bb, c3 ) );
        assert_true ( bb_is_set ( bb, c4 ) );
        assert_true ( bb_is_set ( bb, d5 ) );
        assert_true ( bb_is_set ( bb, e4 ) );
        assert_true ( bb_is_set ( bb, f3 ) );
        assert_true ( bb_is_set ( bb, f4 ) );
        assert_true ( bb_is_set ( bb, h3 ) );
        assert_true ( bb_is_set ( bb, h6 ) );

        bb = brd_get_piece_bb ( brd, BBISHOP );
        assert_true ( bb_is_set ( bb, b1 ) );
        assert_true ( bb_is_set ( bb, f6 ) );

        bb = brd_get_piece_bb ( brd, BKNIGHT );
        assert_true ( bb_is_set ( bb, d1 ) );
        assert_true ( bb_is_set ( bb, b8 ) );

        bb = brd_get_piece_bb ( brd, BROOK );
        assert_true ( bb_is_set ( bb, e1 ) );
        assert_true ( bb_is_set ( bb, g3 ) );

        bb = brd_get_piece_bb ( brd, BQUEEN );
        assert_true ( bb_is_set ( bb, a7 ) );

        pos_destroy ( pos );
}



void test_board_brd_get_colour_bb_black ( void **state )
{
        const char * FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/1bNnrQ2 w - - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( FEN, pos );

        struct board *brd = pos_get_board ( pos );

        bitboard_t bb = brd_get_colour_bb ( brd, BLACK );
        assert_true ( bb_is_set ( bb, c3 ) );
        assert_true ( bb_is_set ( bb, c4 ) );
        assert_true ( bb_is_set ( bb, d5 ) );
        assert_true ( bb_is_set ( bb, e4 ) );
        assert_true ( bb_is_set ( bb, f3 ) );
        assert_true ( bb_is_set ( bb, f4 ) );
        assert_true ( bb_is_set ( bb, h3 ) );
        assert_true ( bb_is_set ( bb, h6 ) );

        assert_true ( bb_is_set ( bb, b1 ) );
        assert_true ( bb_is_set ( bb, f6 ) );

        assert_true ( bb_is_set ( bb, d1 ) );
        assert_true ( bb_is_set ( bb, b8 ) );

        assert_true ( bb_is_set ( bb, e1 ) );
        assert_true ( bb_is_set ( bb, g3 ) );

        assert_true ( bb_is_set ( bb, a7 ) );

        pos_destroy ( pos );
}


void test_board_brd_get_colour_bb_white ( void **state )
{
        const char * FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/1bNnrQ2 w - - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( FEN, pos );

        struct board *brd = pos_get_board ( pos );

        bitboard_t bb = brd_get_colour_bb ( brd, WHITE );

        assert_true ( bb_is_set ( bb, b2 ) );
        assert_true ( bb_is_set ( bb, d3 ) );
        assert_true ( bb_is_set ( bb, e2 ) );
        assert_true ( bb_is_set ( bb, f5 ) );
        assert_true ( bb_is_set ( bb, g2 ) );
        assert_true ( bb_is_set ( bb, g4 ) );
        assert_true ( bb_is_set ( bb, g5 ) );
        assert_true ( bb_is_set ( bb, h2 ) );

        assert_true ( bb_is_set ( bb, b7 ) );
        assert_true ( bb_is_set ( bb, f8 ) );

        assert_true ( bb_is_set ( bb, c1 ) );
        assert_true ( bb_is_set ( bb, e8 ) );

        assert_true ( bb_is_set ( bb, a4 ) );
        assert_true ( bb_is_set ( bb, d8 ) );

        assert_true ( bb_is_set ( bb, f1 ) );

        pos_destroy ( pos );
}

void test_board_brd_get_board_bb ( void **state )
{
        const char * FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/1bNnrQ2 w - - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( FEN, pos );

        bitboard_t bb = brd_get_board_bb ( pos_get_board ( pos ) );

        assert_false ( bb_is_set ( bb, a1 ) );
        assert_false ( bb_is_set ( bb, a2 ) );
        assert_false ( bb_is_set ( bb, a3 ) );
        assert_true ( bb_is_set ( bb, a4 ) );
        assert_false ( bb_is_set ( bb, a5 ) );
        assert_false ( bb_is_set ( bb, a6 ) );
        assert_true ( bb_is_set ( bb, a7 ) );
        assert_false ( bb_is_set ( bb, a8 ) );

        assert_true ( bb_is_set ( bb, b1 ) );
        assert_true ( bb_is_set ( bb, b2 ) );
        assert_false ( bb_is_set ( bb, b3 ) );
        assert_true ( bb_is_set ( bb, b4 ) );
        assert_false ( bb_is_set ( bb, b5 ) );
        assert_true ( bb_is_set ( bb, b6 ) );
        assert_true ( bb_is_set ( bb, b7 ) );
        assert_true ( bb_is_set ( bb, b8 ) );

        assert_true ( bb_is_set ( bb, c1 ) );
        assert_false ( bb_is_set ( bb, c2 ) );
        assert_true ( bb_is_set ( bb, c3 ) );
        assert_true ( bb_is_set ( bb, c4 ) );
        assert_false ( bb_is_set ( bb, c5 ) );
        assert_false ( bb_is_set ( bb, c6 ) );
        assert_false ( bb_is_set ( bb, c7 ) );
        assert_false ( bb_is_set ( bb, c8 ) );

        assert_true ( bb_is_set ( bb, d1 ) );
        assert_false ( bb_is_set ( bb, d2 ) );
        assert_true ( bb_is_set ( bb, d3 ) );
        assert_false ( bb_is_set ( bb, d4 ) );
        assert_true ( bb_is_set ( bb, d5 ) );
        assert_false ( bb_is_set ( bb, d6 ) );
        assert_false ( bb_is_set ( bb, d7 ) );
        assert_true ( bb_is_set ( bb, d8 ) );

        assert_true ( bb_is_set ( bb, e1 ) );
        assert_true ( bb_is_set ( bb, e2 ) );
        assert_false ( bb_is_set ( bb, e3 ) );
        assert_true ( bb_is_set ( bb, e4 ) );
        assert_false ( bb_is_set ( bb, e5 ) );
        assert_false ( bb_is_set ( bb, e6 ) );
        assert_false ( bb_is_set ( bb, e7 ) );
        assert_true ( bb_is_set ( bb, e8 ) );

        assert_true ( bb_is_set ( bb, f1 ) );
        assert_false ( bb_is_set ( bb, f2 ) );
        assert_true ( bb_is_set ( bb, f3 ) );
        assert_true ( bb_is_set ( bb, f4 ) );
        assert_true ( bb_is_set ( bb, f5 ) );
        assert_true ( bb_is_set ( bb, f6 ) );
        assert_false ( bb_is_set ( bb, f7 ) );
        assert_true ( bb_is_set ( bb, f8 ) );

        assert_false ( bb_is_set ( bb, g1 ) );
        assert_true ( bb_is_set ( bb, g2 ) );
        assert_true ( bb_is_set ( bb, g3 ) );
        assert_true ( bb_is_set ( bb, g4 ) );
        assert_true ( bb_is_set ( bb, g5 ) );
        assert_false ( bb_is_set ( bb, g6 ) );
        assert_false ( bb_is_set ( bb, g7 ) );
        assert_false ( bb_is_set ( bb, g8 ) );

        assert_false ( bb_is_set ( bb, h1 ) );
        assert_true ( bb_is_set ( bb, h2 ) );
        assert_true ( bb_is_set ( bb, h3 ) );
        assert_false ( bb_is_set ( bb, h4 ) );
        assert_false ( bb_is_set ( bb, h5 ) );
        assert_true ( bb_is_set ( bb, h6 ) );
        assert_false ( bb_is_set ( bb, h7 ) );
        assert_false ( bb_is_set ( bb, h8 ) );



}

void test_board_brd_is_sq_occupied ( void **state )
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




void test_board_brd_try_get_piece_on_square ( void **state )
{
        const char * FEN = "1n1RNB2/qB6/1k3b1p/3p1PP1/RKp1ppP1/2pP1prp/1P2P1PP/1bNnrQ2 w - - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( FEN, pos );

        struct board *brd = pos_get_board ( pos );

        enum piece pce;

        assert_false ( brd_try_get_piece_on_square ( brd,   a1, &pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   a2 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   a3 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   a4 ,&pce ) );
        assert_true ( pce == WROOK );
        assert_false ( brd_try_get_piece_on_square ( brd,   a5 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   a6 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   a7 ,&pce ) );
        assert_true ( pce == BQUEEN );
        assert_false ( brd_try_get_piece_on_square ( brd,   a8 ,&pce ) );

        assert_true ( brd_try_get_piece_on_square ( brd,   b1 ,&pce ) );
        assert_true ( pce == BBISHOP );
        assert_true ( brd_try_get_piece_on_square ( brd,   b2 ,&pce ) );
        assert_true ( pce == WPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   b3 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   b4 ,&pce ) );
        assert_true ( pce == WKING );
        assert_false ( brd_try_get_piece_on_square ( brd,   b5 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   b6 ,&pce ) );
        assert_true ( pce == BKING );
        assert_true ( brd_try_get_piece_on_square ( brd,   b7 ,&pce ) );
        assert_true ( pce == WBISHOP );
        assert_true ( brd_try_get_piece_on_square ( brd,   b8 ,&pce ) );
        assert_true ( pce == BKNIGHT );

        assert_true ( brd_try_get_piece_on_square ( brd,   c1 ,&pce ) );
        assert_true ( pce == WKNIGHT );
        assert_false ( brd_try_get_piece_on_square ( brd,   c2 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   c3 ,&pce ) );
        assert_true ( pce == BPAWN );
        assert_true ( brd_try_get_piece_on_square ( brd,   c4 ,&pce ) );
        assert_true ( pce == BPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   c5 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   c6 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   c7 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   c8 ,&pce ) );

        assert_true ( brd_try_get_piece_on_square ( brd,   d1 ,&pce ) );
        assert_true ( pce == BKNIGHT );
        assert_false ( brd_try_get_piece_on_square ( brd,   d2 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   d3 ,&pce ) );
        assert_true ( pce == WPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   d4 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   d5 ,&pce ) );
        assert_true ( pce == BPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   d6 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   d7 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   d8 ,&pce ) );
        assert_true ( pce == WROOK );

        assert_true ( brd_try_get_piece_on_square ( brd,   e1 ,&pce ) );
        assert_true ( pce == BROOK );
        assert_true ( brd_try_get_piece_on_square ( brd,   e2 ,&pce ) );
        assert_true ( pce == WPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   e3 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   e4 ,&pce ) );
        assert_true ( pce == BPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   e5 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   e6 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   e7 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   e8 ,&pce ) );
        assert_true ( pce == WKNIGHT );

        assert_true ( brd_try_get_piece_on_square ( brd,   f1 ,&pce ) );
        assert_true ( pce == WQUEEN );
        assert_false ( brd_try_get_piece_on_square ( brd,   f2 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   f3 ,&pce ) );
        assert_true ( pce == BPAWN );
        assert_true ( brd_try_get_piece_on_square ( brd,   f4 ,&pce ) );
        assert_true ( pce == BPAWN );
        assert_true ( brd_try_get_piece_on_square ( brd,   f5 ,&pce ) );
        assert_true ( pce == WPAWN );
        assert_true ( brd_try_get_piece_on_square ( brd,   f6 ,&pce ) );
        assert_true ( pce == BBISHOP );
        assert_false ( brd_try_get_piece_on_square ( brd,   f7 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   f8 ,&pce ) );
        assert_true ( pce == WBISHOP );

        assert_false ( brd_try_get_piece_on_square ( brd,   g1 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   g2 ,&pce ) );
        assert_true ( pce == WPAWN );
        assert_true ( brd_try_get_piece_on_square ( brd,   g3 ,&pce ) );
        assert_true ( pce == BROOK );
        assert_true ( brd_try_get_piece_on_square ( brd,   g4 ,&pce ) );
        assert_true ( pce == WPAWN );
        assert_true ( brd_try_get_piece_on_square ( brd,   g5 ,&pce ) );
        assert_true ( pce == WPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   g6 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   g7 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   g8 ,&pce ) );

        assert_false ( brd_try_get_piece_on_square ( brd,   h1 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   h2 ,&pce ) );
        assert_true ( pce == WPAWN );
        assert_true ( brd_try_get_piece_on_square ( brd,   h3 ,&pce ) );
        assert_true ( pce == BPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   h4 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   h5 ,&pce ) );
        assert_true ( brd_try_get_piece_on_square ( brd,   h6 ,&pce ) );
        assert_true ( pce == BPAWN );
        assert_false ( brd_try_get_piece_on_square ( brd,   h7 ,&pce ) );
        assert_false ( brd_try_get_piece_on_square ( brd,   h8 ,&pce ) );

}



static bool is_sq_in_list ( const enum square * sqlist, const int list_size, const enum square sq_to_check )
{
        for ( int i = 0; i < list_size; i++ ) {
                if ( * ( sqlist + i ) == sq_to_check ) {
                        return true;
                }
        }
        return false;
}
// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

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

void test_move_gen_knight_white_1 ( void **state )
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


void test_move_gen_knight_white_2 ( void **state )
{
        const char *RANDOM_FEN_1 = "1N3b1k/1bRRPrp1/QP1P2B1/1P1P2Bn/pK1p3p/2P1Np1p/1pPPpq1n/4r3 w - - 0 1\n";
        const uint16_t NUM_EXPECTED_WHITE_KNIGHT_MOVES = 7;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct board *brd = pos_get_board ( pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_knight_moves ( brd, WHITE, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_WHITE_KNIGHT_MOVES );

        // check quiet moves
        move_t mv = move_encode_quiet ( b8, c6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e3, c4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e3, d1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e3, f1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e3, g2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e3, g4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e3, f5 );

        // no capture moves
}

void test_move_gen_knight_white_3 ( void **state )
{
        const char *RANDOM_FEN_1 = "K3b1nk/3p3p/2pPNqP1/1pQPp1PB/1p4Pp/pP6/P1PRrN1B/3nb1Rr w - - 0 1\n";
        const uint16_t NUM_EXPECTED_WHITE_KNIGHT_MOVES = 11;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct board *brd = pos_get_board ( pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_knight_moves ( brd, WHITE, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_WHITE_KNIGHT_MOVES );

        // check quiet moves
        move_t mv = move_encode_quiet ( f2, e4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f2, d3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f2, h3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e6, c7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e6, d4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e6, d8 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e6, f4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e6, g7 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( e6, f8 );
        assert_true ( mvl_contains_move ( mvl, mv ) );


        // capture moves
        mv = move_encode_capture ( f2, d1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( f2, h1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

}




void test_move_gen_knight_black_1 ( void **state )
{
        const char *RANDOM_FEN_1 = "R1n2b2/3p4/K1P2n2/1P2N2p/P2k1pN1/1P2p1Q1/Rpb1p3/1rB5 b - - 0 1\n";
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
        assert_true ( mvl_contains_move ( mvl, mv ) );
}


void test_move_gen_knight_black_2 ( void **state )
{
        const char *RANDOM_FEN_1 = "1r5r/1P1PPP2/ppB1KN2/1p5q/2nR1PPP/p3Pnpp/kpNRQ1pb/4B2b b - - 0 1\n";
        const uint16_t NUM_EXPECTED_BLACK_KNIGHT_MOVES = 12;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct board *brd = pos_get_board ( pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_knight_moves ( brd, BLACK, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_BLACK_KNIGHT_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( c4, a5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c4, e5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c4, d6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f3, g1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f3, g5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f3, e5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        // check capture moves
        mv = move_encode_capture ( c4, e3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( c4, d2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( f3, e1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( f3, d2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( f3, d4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( f3, h4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
}



void test_move_gen_knight_black_3 ( void **state )
{
        const char *RANDOM_FEN_1 = "2K5/ppbQpr1q/N2PPBP1/kB1bP1N1/2P4p/1pP4p/1npnPPp1/rR2R3 b - - 0 1\n";
        const uint16_t NUM_EXPECTED_BLACK_KNIGHT_MOVES = 9;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct board *brd = pos_get_board ( pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_knight_moves ( brd, BLACK, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_BLACK_KNIGHT_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( b2, d1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( b2, d3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( b2, a4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( d2, f1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( d2, f3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( d2, e4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        // check capture moves
        mv = move_encode_capture ( b2, c4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( d2, c4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_capture ( d2, b1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

}






void test_move_gen_king_white_no_castling_1 ( void **state )
{
        const char *RANDOM_FEN_1 = "5Q2/1p3pp1/3P1Pn1/6k1/2K5/Pr3p2/p2R1P2/7B w - - 0 1\n";
        const uint16_t NUM_EXPECTED_MOVES = 8;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, WHITE, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( c4, c3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c4, d3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c4, d4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c4, d5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c4, c5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c4, b5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( c4, b4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        // check capture moves
        mv = move_encode_capture ( c4, b3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
}


void test_move_gen_king_white_no_castling_2 ( void **state )
{
        const char *RANDOM_FEN_1 = "8/3Q2pN/1b5P/3Pp2K/2n3p1/P2RP3/Pp6/5k2 w - - 0 1\n";
        const uint16_t NUM_EXPECTED_MOVES = 4;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, WHITE, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( h5, g6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( h5, g5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( h5, h4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        // check capture moves
        mv = move_encode_capture ( h5, g4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
}



void test_move_gen_king_white_no_castling_3 ( void **state )
{
        const char *RANDOM_FEN_1 = "n7/4p2p/PP1p1pp1/1n4k1/6q1/6N1/K3p3/2N3r1 w - - 0 1\n";
        const uint16_t NUM_EXPECTED_MOVES = 5;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, WHITE, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( a2, a3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( a2, b3 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( a2, b2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( a2, b1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( a2, a1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

}

void test_move_white_castling_WK_WQ ( void **state )
{
        const char *RANDOM_FEN_1 = "r1bqkb1r/ppp2ppp/2n2n2/3pp3/2PP4/2NQPN2/PP1BBPPP/R3K2R w KQkq - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, WHITE, mvl );

        move_t wk_cast = move_encode_castle_kingside_white();
        assert_true ( mvl_contains_move ( mvl, wk_cast ) );

        move_t wq_cast = move_encode_castle_queenside_white();
        assert_true ( mvl_contains_move ( mvl, wq_cast ) );
}



void test_move_white_castling_WK_only ( void **state )
{
        const char *RANDOM_FEN_1 = "r1bqkb1r/ppp2ppp/2n2n2/3pp3/2PP4/2NQPN2/PP2BPPP/R1B1K2R w K - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, WHITE, mvl );

        move_t wk_cast = move_encode_castle_kingside_white();
        assert_true ( mvl_contains_move ( mvl, wk_cast ) );

        move_t wq_cast = move_encode_castle_queenside_white();
        assert_false ( mvl_contains_move ( mvl, wq_cast ) );
}


void test_move_white_castling_WQ_only ( void **state )
{
        const char *RANDOM_FEN_1 = "r1bqkb1r/ppp2ppp/2n2n2/3pp3/2PP4/2NQP3/PP1B1PPP/R3K1NR w Q - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, WHITE, mvl );

        move_t wk_cast = move_encode_castle_kingside_white();
        assert_false ( mvl_contains_move ( mvl, wk_cast ) );

        move_t wq_cast = move_encode_castle_queenside_white();
        assert_true ( mvl_contains_move ( mvl, wq_cast ) );
}



void test_move_gen_king_black_no_castling_1 ( void **state )
{
        const char *RANDOM_FEN_1 = "5Q2/1p3pp1/3P1Pn1/6k1/2K5/Pr3p2/p2R1P2/7B w - - 0 1\n";
        const uint16_t NUM_EXPECTED_MOVES = 7;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, BLACK, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( g5, h6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, h5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, h4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, g4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, f4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, f5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

        // check capture moves
        mv = move_encode_capture ( g5, f6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
}


void test_move_gen_king_black_no_castling_2 ( void **state )
{
        const char *RANDOM_FEN_1 = "8/3Q2pN/1b5P/3Pp2K/2n3p1/P2RP3/Pp6/5k2 w - - 0 1\n";
        const uint16_t NUM_EXPECTED_MOVES = 5;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, BLACK, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( f1, g1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f1, g2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f1, f2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f1, e2 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( f1, e1 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

}



void test_move_gen_king_black_no_castling_3 ( void **state )
{
        const char *RANDOM_FEN_1 = "n7/4p2p/PP1p1pp1/1n4k1/6q1/6N1/K3p3/2N3r1 w - - 0 1\n";
        const uint16_t NUM_EXPECTED_MOVES = 5;

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, BLACK, mvl );

        assert_true ( mvl_get_move_count ( mvl ) == NUM_EXPECTED_MOVES );


        // check quiet moves
        move_t mv = move_encode_quiet ( g5, h6 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, h5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, h4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, f4 );
        assert_true ( mvl_contains_move ( mvl, mv ) );
        mv = move_encode_quiet ( g5, f5 );
        assert_true ( mvl_contains_move ( mvl, mv ) );

}


void test_move_black_castling_BK_BQ ( void **state )
{
        const char *RANDOM_FEN_1 = "r3k2r/pppqbppp/2n1bn2/3pp3/2PP4/2NQP3/PP1B1PPP/R3K1NR b Qkq - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, BLACK, mvl );

        move_t bk_cast = move_encode_castle_kingside_black();
        assert_true ( mvl_contains_move ( mvl, bk_cast ) );

        move_t bq_cast = move_encode_castle_queenside_black();
        assert_true ( mvl_contains_move ( mvl, bq_cast ) );
}




void test_move_black_castling_BK_only ( void **state )
{
        const char *RANDOM_FEN_1 = "rn2k2r/pppqbppp/4bn2/3pp3/2PP4/2NQP3/PP1B1PPP/R3K1NR w Qk - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, BLACK, mvl );

        move_t bk_cast = move_encode_castle_kingside_black();
        assert_true ( mvl_contains_move ( mvl, bk_cast ) );

        move_t bq_cast = move_encode_castle_queenside_black();
        assert_false ( mvl_contains_move ( mvl, bq_cast ) );
}


void test_move_black_castling_BQ_only ( void **state )
{
        const char *RANDOM_FEN_1 = "r3k1nr/pppqbppp/2n1b3/3pp3/2PP4/2NQP3/PP1B1PPP/R3K1NR w Qq - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_king_moves ( pos, BLACK, mvl );

        move_t bk_cast = move_encode_castle_kingside_black();
        assert_false ( mvl_contains_move ( mvl, bk_cast ) );

        move_t bq_cast = move_encode_castle_queenside_black();
        assert_true ( mvl_contains_move ( mvl, bq_cast ) );
}




// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

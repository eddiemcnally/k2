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
#include "square.h"
#include "piece.h"
#include "move.h"
#include "test_move.h"



void test_move_quiet_move_encode_decode ( void **state )
{
        for ( enum square from_sq = a1; from_sq <= h8; from_sq++ ) {
                for ( enum square to_sq = a1; to_sq <= h8; to_sq++ ) {
                        if ( from_sq == to_sq ) {
                                continue;
                        }

                        const uint16_t mv = move_encode_quiet ( from_sq, to_sq );

                        enum square decoded_from = move_decode_from_sq ( mv );
                        enum square decoded_to = move_decode_to_sq ( mv );

                        assert_true ( from_sq == decoded_from );
                        assert_true ( to_sq == decoded_to );

                        bool is_quiet = move_is_quiet ( mv );
                        assert_true ( is_quiet );
                }
        }
}



void test_move_promoted_capture_move_encode_decode ( void **state )
{
        for ( enum square from_sq = a1; from_sq <= h8; from_sq++ ) {
                for ( enum square to_sq = a1; to_sq <= h8; to_sq++ ) {
                        if ( from_sq == to_sq ) {
                                continue;
                        }

                        uint16_t mv = move_encode_promoted ( from_sq, to_sq, WQUEEN, true );

                        enum square decoded_from = move_decode_from_sq ( mv );
                        enum square decoded_to = move_decode_to_sq ( mv );

                        assert_true ( from_sq == decoded_from );
                        assert_true ( to_sq == decoded_to );

                        bool is_capture = move_is_capture ( mv );
                        assert_true ( is_capture );
                }
        }
}

void test_move_promoted_non_capture_move_encode_decode ( void **state )
{
        for ( enum square from_sq = a1; from_sq <= h8; from_sq++ ) {
                for ( enum square to_sq = a1; to_sq <= h8; to_sq++ ) {
                        if ( from_sq == to_sq ) {
                                continue;
                        }

                        uint16_t mv = move_encode_promoted ( from_sq, to_sq, WQUEEN, false );

                        enum square decoded_from = move_decode_from_sq ( mv );
                        enum square decoded_to = move_decode_to_sq ( mv );

                        assert_true ( from_sq == decoded_from );
                        assert_true ( to_sq == decoded_to );

                        bool is_capture = move_is_capture ( mv );
                        assert_false ( is_capture );
                }
        }
}


void test_move_en_passant_move_encode_decode ( void **state )
{
        for ( enum square from_sq = a1; from_sq <= h8; from_sq++ ) {
                for ( enum square to_sq = a1; to_sq <= h8; to_sq++ ) {
                        if ( from_sq == to_sq ) {
                                continue;
                        }

                        uint16_t mv = move_encode_enpassant ( from_sq, to_sq );

                        enum square decoded_from = move_decode_from_sq ( mv );
                        enum square decoded_to = move_decode_to_sq ( mv );

                        assert_true ( from_sq == decoded_from );
                        assert_true ( to_sq == decoded_to );

                        bool is_capture = move_is_capture ( mv );
                        assert_true ( is_capture );

                        bool is_enpassant = move_is_en_passant ( mv );
                        assert_true ( is_enpassant );
                }
        }
}


void test_move_decode_promotion_piece_white ( void **state )
{
        const enum square from_sq = a7;
        const enum square to_sq = a8;

        uint16_t mv = move_encode_promoted ( from_sq, to_sq, WKNIGHT, true );
        enum piece pce = move_decode_promotion_piece ( mv, WHITE );
        assert_true ( pce == WKNIGHT );

        mv = move_encode_promoted ( from_sq, to_sq, WBISHOP, true );
        pce = move_decode_promotion_piece ( mv, WHITE );
        assert_true ( pce == WBISHOP );

        mv = move_encode_promoted ( from_sq, to_sq, WROOK, true );
        pce = move_decode_promotion_piece ( mv, WHITE );
        assert_true ( pce == WROOK );

        mv = move_encode_promoted ( from_sq, to_sq, WQUEEN, true );
        pce = move_decode_promotion_piece ( mv, WHITE );
        assert_true ( pce == WQUEEN );

}

void test_move_decode_promotion_piece_black ( void **state )
{
        const enum square from_sq = a7;
        const enum square to_sq = a8;

        uint16_t mv = move_encode_promoted ( from_sq, to_sq, BKNIGHT, true );
        enum piece pce = move_decode_promotion_piece ( mv, BLACK );
        assert_true ( pce == BKNIGHT );

        mv = move_encode_promoted ( from_sq, to_sq, BBISHOP, true );
        pce = move_decode_promotion_piece ( mv, BLACK );
        assert_true ( pce == BBISHOP );

        mv = move_encode_promoted ( from_sq, to_sq, BROOK, true );
        pce = move_decode_promotion_piece ( mv, BLACK );
        assert_true ( pce == BROOK );

        mv = move_encode_promoted ( from_sq, to_sq, BQUEEN, true );
        pce = move_decode_promotion_piece ( mv, BLACK );
        assert_true ( pce == BQUEEN );
}


void test_move_is_king_side_castle_white ( void **state )
{
        uint16_t mv = move_encode_castle_kingside_white();
        assert_true ( move_is_king_castle ( mv ) );
}

void test_move_is_king_side_castle_black ( void **state )
{
        uint16_t mv = move_encode_castle_kingside_black();
        assert_true ( move_is_king_castle ( mv ) );
}


void test_move_is_queen_side_castle_white ( void **state )
{
        uint16_t mv = move_encode_castle_queenside_white();
        assert_true ( move_is_queen_castle ( mv ) );
}

void test_move_is_queen_side_castle_black ( void **state )
{
        uint16_t mv = move_encode_castle_queenside_black();
        assert_true ( move_is_queen_castle ( mv ) );
}


void test_move_is_double_pawn_white ( void **state )
{
        for ( enum square from_sq = a2; from_sq <= h2; from_sq++ ) {

                const enum square to_sq = ( enum square ) ( from_sq + 16 );
                uint16_t mv = move_encode_pawn_double_first ( from_sq, to_sq );

                assert_true ( move_is_double_pawn ( mv ) );
        }
}



void test_move_is_double_pawn_black ( void **state )
{
        for ( enum square from_sq = a7; from_sq <= h7; from_sq++ ) {

                const enum square to_sq = ( enum square ) ( from_sq - 16 );
                uint16_t mv = move_encode_pawn_double_first ( from_sq, to_sq );

                assert_true ( move_is_double_pawn ( mv ) );
        }
}



void test_move_get_promote_piece_white ( void **state )
{
        const enum square from_sq = e7;
        const enum square to_sq = e8;

        enum piece test_pieces[4] = {WKNIGHT, WBISHOP, WROOK, WQUEEN};

        for ( int i = 0; i < 4; i++ ) {
                enum piece pce = test_pieces[i];

                uint16_t mv = move_encode_promoted ( from_sq, to_sq, pce, true );
                assert_true ( move_get_promote_piece ( mv, WHITE ) == pce );

                mv = move_encode_promoted ( from_sq, to_sq, pce, false );
                assert_true ( move_get_promote_piece ( mv, WHITE ) == pce );

        }
}


void test_move_get_promote_piece_black ( void **state )
{
        const enum square from_sq = e2;
        const enum square to_sq = e1;

        enum piece test_pieces[4] = {BKNIGHT, BBISHOP, BROOK, BQUEEN};

        for ( int i = 0; i < 4; i++ ) {
                enum piece pce = test_pieces[i];

                uint16_t mv = move_encode_promoted ( from_sq, to_sq, pce, true );
                assert_true ( move_get_promote_piece ( mv, BLACK ) == pce );

                mv = move_encode_promoted ( from_sq, to_sq, pce, false );
                assert_true ( move_get_promote_piece ( mv, BLACK ) == pce );

        }
}


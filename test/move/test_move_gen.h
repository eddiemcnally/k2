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

#pragma once

void test_move_gen_knight_white_1 ( void **state );
void test_move_gen_knight_white_2 ( void **state );
void test_move_gen_knight_white_3 ( void **state );
void test_move_gen_knight_black_1 ( void **state );
void test_move_gen_knight_black_2 ( void **state );
void test_move_gen_knight_black_3 ( void **state );
void test_move_gen_king_white_no_castling_1 ( void **state );
void test_move_gen_king_white_no_castling_2 ( void **state );
void test_move_gen_king_white_no_castling_3 ( void **state );
void test_move_gen_king_black_no_castling_1 ( void **state );
void test_move_gen_king_black_no_castling_2 ( void **state );
void test_move_gen_king_black_no_castling_3 ( void **state );

void test_move_white_castling_WK_WQ ( void **state );
void test_move_white_castling_WK_only ( void **state );
void test_move_white_castling_WQ_only ( void **state );
void test_move_black_castling_BK_BQ ( void **state );
void test_move_black_castling_BK_only ( void **state );
void test_move_black_castling_BQ_only ( void **state );

void test_move_white_pawns_promotion_1 ( void **state );
void test_move_white_pawns_promotion_2 ( void **state );
void test_move_white_pawns_1 ( void **state );
void test_move_white_pawns_2 ( void **state );
void test_move_white_pawns_first_move_double_1 ( void **state );
void test_move_white_pawns_first_move_double_2 ( void **state );
void test_move_white_pawns_en_passant_1 ( void **state );
void test_move_white_pawns_en_passant_2 ( void **state );
void test_move_white_pawns_en_passant_3 ( void **state );

void test_move_black_pawns_promotion_1 ( void **state );
void test_move_black_pawns_promotion_2 ( void **state );
void test_move_black_pawns_1 ( void **state );
void test_move_black_pawns_2 ( void **state );
void test_move_black_pawns_first_move_double_1 ( void **state );
void test_move_black_pawns_first_move_double_2 ( void **state );
void test_move_black_pawns_en_passant_1 ( void **state );
void test_move_black_pawns_en_passant_2 ( void **state );
void test_move_black_pawns_en_passant_3 ( void **state );


// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

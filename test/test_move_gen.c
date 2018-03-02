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

void test_knight_move_gen ( void **state )
{
        const char *RANDOM_FEN_1 = "R1n2b2/3p4/K1P2n2/1P2N2p/P2k1pN1/1P2p1Q1/Rpb1p3/1rB5 w - - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise(RANDOM_FEN_1, pos);
        struct board *brd = pos_get_board(pos);
        struct move_list* mvl = mvl_allocate();
        
        mv_gen_knight_moves(brd, WHITE, mvl);
          
        // check quiet moves
        move_t mv = move_encode_quiet(g4, f2);
        assert_true(mvl_contains_move(mvl, mv));
        mv = move_encode_quiet(g4, h2);
        assert_true(mvl_contains_move(mvl, mv));
        mv = move_encode_quiet(g4, h6);
        assert_true(mvl_contains_move(mvl, mv));
        mv = move_encode_quiet(e5, c4);
        assert_true(mvl_contains_move(mvl, mv));
        mv = move_encode_quiet(e5, d3);
        assert_true(mvl_contains_move(mvl, mv));
        mv = move_encode_quiet(e5, f3);
        assert_true(mvl_contains_move(mvl, mv));
        mv = move_encode_quiet(e5, g6);
        assert_true(mvl_contains_move(mvl, mv));
        mv = move_encode_quiet(e5, f7);
        assert_true(mvl_contains_move(mvl, mv));
        
        
        
/*        quiet
g4->f2
g4->h2
g4->h6
e5->c4
e5->d3
e5->f3
e5->g6
e5->f7

captures
g4->e3
g4->f6
e5->d7
*/
        
        
        // check quiet moves
        





}


// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

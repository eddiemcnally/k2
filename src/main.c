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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "square.h"
#include "position.h"
#include "move_gen.h"
#include "move_list.h"
#include "piece.h"
#include "occ_mask_gen.h"


//#define VERSION_MAJOR	0
//#define VERSION_MINOR	1

int main ( void )
{

        const char *RANDOM_FEN_1 = "R1n2b2/3p4/K1P2n2/1P2N2p/P2k1pN1/1P2p1Q1/Rpb1p3/1rB5 w - - 0 1\n";

        struct position *pos = pos_create();
        pos_initialise ( RANDOM_FEN_1, pos );
        struct board *brd = pos_get_board ( pos );
        struct move_list* mvl = mvl_allocate();

        mv_gen_knight_moves ( brd, WHITE, mvl );


}
// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

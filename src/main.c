/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without 
 *  restriction, including without limitation the rights to use, 
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following 
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "board.h"
#include "move.h"
#include "move_gen.h"
#include "move_list.h"
#include "perft.h"
#include "perft_file_reader.h"
#include "piece.h"
#include "position.h"
#include "search.h"
#include "square.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//#define VERSION_MAJOR 0
//#define VERSION_MINOR 1

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
int main(void) {

    //mate in 3
    //soluiton : 1.Ra6 f6 2.Bxf6 Rg7 3.Rxa8#
    //https://www.sparkchess.com/chess-puzzles/for-beginners-mate-in-three.html
    const char *MATE_IN_THREE = "r5rk/5p1p/5R2/4B3/8/8/7P/7K w - - 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(MATE_IN_THREE, pos);

    struct search_data info = {0};
    info.search_depth = 5;

    search_position(pos, &info);

    pos_destroy(pos);
}

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
#include "occ_mask_gen.h"
#include "perft.h"
#include "piece.h"
#include "position.h"
#include "square.h"
#include "utils.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//#define VERSION_MAJOR 0
//#define VERSION_MINOR 1

int main(void) {

    // const enum square from_sq = a7;
    // const enum square to_sq = a8;
    // struct move mv;
    // enum piece pce;

    // // mv = move_encode_promoted(from_sq, to_sq, KNIGHT, true);

    // // try_move_decode_promotion_piece(mv, WHITE, &pce);

    // // printf("pce = 0x%x\n", pce);

    // // enum piece_role role = pce_get_piece_role(pce);
    // // printf("role = 0x%x\n", role);

    const char *RANDOM_FEN_1 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq – 0 1\n";

    struct position *pos = pos_create();
    pos_initialise(RANDOM_FEN_1, pos);

    const double start_time_in_secs = get_time_of_day_in_secs();

    do_perft(5, pos);

    const double elapsed_in_secs = get_elapsed_time_in_secs(start_time_in_secs);
    printf("Elapsed time : %f\n", elapsed_in_secs);
}

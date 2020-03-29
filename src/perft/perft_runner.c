/*  MIT License
 *
 *  Copyright (c) 2020 Eddie McNally
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

#include "perft_runner.h"
#include "move_gen.h"
#include "perft_file_reader.h"
#include "utils.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint64_t do_perft(const uint8_t depth, struct position *pos);

int main(void) {

    struct perft_epd parsed = perft_load_file("perftsuite.epd");
    uint64_t total_nodes = 0;

    for (int r = 0; r < parsed.row_count; r++) {
        struct position *pos = pos_create();
        pos_initialise(parsed.rows[r].fen, pos);

        for (int d = 0; d < PERFT_EPD_MAX_DEPTH; d++) {
            const uint64_t expected_nodes = parsed.rows[r].move_cnt[d];

            const uint64_t actual_nodes = do_perft((uint8_t)(d + 1), pos);
            total_nodes += actual_nodes;

            if (expected_nodes != actual_nodes) {
                printf(
                    "*** Problem: fen=%s, depth=%d, expected=%lu, actual=%lu\n",
                    parsed.rows[r].fen, (d + 1), expected_nodes, actual_nodes);
            }
            printf("fen=%s, depth=%d, expected=%lu \n",
                   parsed.rows[r].fen, (d + 1), expected_nodes);
        }
    }

    printf("Total node count: %lu\n", total_nodes);
}

uint64_t do_perft(const uint8_t depth, struct position *pos) {
    if (depth == 0) {
        return 1;
    }

    uint64_t nodes = 0;

    struct move_list mvl = mvl_initialise();
    mv_gen_all_moves(pos, &mvl);

    //printf("generated move cnt %d\n", mvl.move_count);

    for (int i = 0; i < mvl.move_count; i++) {
        const struct move mv = mvl.move_list[i];
        const bool is_legal = pos_try_make_move(pos, mv);

        if (is_legal) {
            nodes += do_perft(depth - 1, pos);
        }
        pos_take_move(pos);
    }

    return nodes;
}

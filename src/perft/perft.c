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

/*! @addtogroup Perft
 *
 * @ingroup Perft
 * @{
 * @details Perft recursive move maker
 *
 */

#include "perft.h"
#include "move_gen.h"
#include "perft_file_reader.h"
#include "position.h"
#include "utils.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
        const enum move_legality legality = pos_make_move(pos, mv);

        if (legality == LEGAL_MOVE) {
            nodes += do_perft(depth - 1, pos);
        }
        pos_take_move(pos);
    }

    return nodes;
}

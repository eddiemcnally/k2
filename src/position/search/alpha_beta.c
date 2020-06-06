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

/*! @addtogroup Search
 *
 * @ingroup Alpha-Beta
 * @{
 * @details Implements the Alpha-Beta search function
 *
 */

#include "alpha_beta.h"
#include "move_gen.h"
#include "move_list.h"

int32_t alpha_beta_search(int32_t alpha, int32_t beta, uint8_t depth, struct position *pos,
                          struct search_data *search_info) {

    if (depth == 0) {
        // do quiesence search
        return 0;
    }

    // todo:
    // check repetition
    // check 50 move rule
    // check max depth
    // check search_info->node count > MAX COUNT

    struct move_list mv_list = mvl_initialise();

    mv_gen_all_moves(pos, &mv_list);
}

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

#include "pv_table.h"
#include "move_gen.h"
#include "move_list.h"

#define PV_TABLE_SIZE_IN_ENTRIES 200000000
static bool move_exists(struct position *pos, const struct move mv);

static struct move pvtable[PV_TABLE_SIZE_IN_ENTRIES];

void pv_table_init(void) {
    for (int i = 0; i < PV_TABLE_SIZE_IN_ENTRIES; i++) {
        pvtable[i] = move_get_no_move();
    }
}

enum pvt_add_condition pv_table_add(const uint64_t position_hash, const struct move mv) {
    const uint32_t idx = position_hash % PV_TABLE_SIZE_IN_ENTRIES;

    enum pvt_add_condition retval = PVT_ADD_OK;
    struct move existing = pvtable[idx];

    if (move_compare(existing, move_get_no_move()) == false) {
        retval = PVT_ADD_WITH_COLLISION;
    }

    pvtable[idx] = mv;
    return retval;
}

bool pv_table_contains_position(const uint64_t position_hash) {
    const struct move mv = pv_table_get_move(position_hash);
    return !move_compare(mv, move_get_no_move());
}

struct move pv_table_get_move(const uint64_t position_hash) {
    const uint32_t idx = (uint32_t)(position_hash % PV_TABLE_SIZE_IN_ENTRIES);
    return pvtable[idx];
}

struct pv_line pv_table_get_pv_line(uint8_t depth, struct position *pos) {

    struct pv_line retval;

    struct move mv = pv_table_get_move(pos_get_hash(pos));
    uint8_t i = 0;

    while (!move_compare(mv, move_get_no_move()) && i < depth) {
        if (move_exists(pos, mv)) {
            pos_make_move(pos, mv);
            retval.line[i] = mv;
            i++;
        } else {
            break;
        }
        mv = pv_table_get_move(pos_get_hash(pos));
    }

    while (pos_get_ply(pos) > 0) {
        pos_take_move(pos);
    }

    return retval;
}

static bool move_exists(struct position *pos, const struct move mv) {
    struct move_list mvl = mvl_initialise();

    mv_gen_all_moves(pos, &mvl);

    for (int i = 0; i < mvl.move_count; i++) {
        enum move_legality legal = pos_make_move(pos, mvl.move_list[i]);
        if (legal != LEGAL_MOVE) {
            continue;
        }
        pos_take_move(pos);

        if (move_compare(mvl.move_list[i], mv)) {
            return true;
        }
    }
    return false;
}

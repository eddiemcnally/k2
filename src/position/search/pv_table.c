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
#include "move.h"

#define PV_TABLE_SIZE_IN_ENTRIES 200000000

static struct move pvtable[PV_TABLE_SIZE_IN_ENTRIES] = {0};

void pv_table_init(void) {
    for (int i = 0; i < PV_TABLE_SIZE_IN_ENTRIES; i++) {
        pvtable[i] = move_get_no_move();
    }
}

void pv_table_add(const uint64_t position_hash, const struct move mv) {
    const uint32_t idx = position_hash % PV_TABLE_SIZE_IN_ENTRIES;
    pvtable[idx] = mv;
}

bool pv_table_contains_position(const uint64_t position_hash) {
    const uint32_t idx = (uint32_t)(position_hash % PV_TABLE_SIZE_IN_ENTRIES);
    return !move_compare(pvtable[idx], move_get_no_move());
}

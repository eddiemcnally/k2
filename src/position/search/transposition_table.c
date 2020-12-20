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
 * @ingroup Transposition_Table
 * @{
 * @details Used to maintain the sequence of best moves
 *
 */

#include <assert.h>

#include "hashkeys.h"
#include "transposition_table.h"
#include "utils.h"

static void set_tt_size(uint64_t size_in_bytes);
static inline void init_table(void);

struct tt_entry {
    uint64_t position_hash;
    struct move mv;
    uint8_t depth;
    bool slot_used;
};

// num entries in TT
static uint32_t num_tt_elems = 0;
// ptr to transposition table
static struct tt_entry *tt = NULL;

void tt_create(uint64_t size_in_bytes) {
    if (tt != NULL) {
        tt_dispose();
    }

    set_tt_size(size_in_bytes);

    init_table();
}

uint32_t tt_capacity(void) {
    return num_tt_elems;
}

size_t tt_entry_size(void) {
    return sizeof(struct tt_entry);
}

bool tt_probe_position(const uint64_t position_hash, struct move *mv) {
    const struct tt_entry *elem = &tt[position_hash & num_tt_elems];

    if (elem->position_hash == position_hash) {
        *mv = elem->mv;
        return true;
    }
    return false;
}

bool tt_add(const uint64_t position_hash, const struct move mv, uint8_t depth) {
    assert(validate_move(mv));
    assert(tt != NULL);
    assert(num_tt_elems > 0);

    struct tt_entry *entry = &tt[position_hash & num_tt_elems];

    if (entry->slot_used == true) {
        // slot is filled, only add if depth is greater
        if (entry->depth > depth) {
            return false;
        }
    }
    entry->position_hash = position_hash;
    entry->mv = mv;
    entry->depth = depth;
    entry->slot_used = true;

    return true;
}

void tt_dispose(void) {
    if (tt != NULL) {
        free(tt);

        tt = NULL;
    }
}

static void set_tt_size(uint64_t size_in_bytes) {
    uint64_t adj_size = round_down_to_nearest_power_2(size_in_bytes);

    if (adj_size < sizeof(struct tt_entry)) {
        num_tt_elems = 0;
        return;
    }

    num_tt_elems = (uint32_t)((adj_size / sizeof(struct tt_entry)) - 1);
    tt = (struct tt_entry *)calloc(num_tt_elems, sizeof(struct tt_entry));
}

static inline void init_table(void) {
    for (uint64_t i = 0; i < num_tt_elems; i++) {
        struct tt_entry *entry = &tt[i];
        entry->slot_used = false;
    }
}

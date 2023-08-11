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
#include "search.h"
#include "transposition_table.h"
#include "utils.h"

#define MIN_NUM_TT_SLOTS 1000000

static void set_tt_size(uint64_t size_in_bytes);
static void init_table(void);
static uint32_t get_index(const uint64_t hash);
static bool validate_node_type(const enum node_type nt);

struct tt_entry {
    uint64_t position_hash;
    struct move mv;
    int32_t score;
    uint8_t depth;
    bool slot_used;
    enum node_type node_type;
};

// num entries in TT
static uint32_t num_tt_elems = 0;
// ptr to transposition table
static struct tt_entry *tt = NULL;

/**
 * @brief Create an initialise the Transposition Table
 * 
 * @param size_in_bytes The size in bytes of the Transposition Table
 */
void tt_create(uint64_t size_in_bytes) {
    if (size_in_bytes < sizeof(struct tt_entry)) {
        printf("Required TT size is too small...setting to %d\n", MIN_NUM_TT_SLOTS);
    }

    if (tt != NULL) {
        tt_dispose();
    }

    set_tt_size(size_in_bytes);

    init_table();
}

/**
 * @brief Returns the number of TT elements in the table
 * 
 * @return uint32_t The number of elements
 */
uint32_t tt_capacity(void) {
    return num_tt_elems;
}

size_t tt_entry_size(void) {
    return sizeof(struct tt_entry);
}

/**
 * @brief Adds the given search info to the TT table
 * 
 * @param position_hash The position hash
 * @param mv The move
 * @param depth The search depth
 * @param score The score associated with the position after the move
 * @param node_type The node type
 * @return true if position info was added to TT
 * @return false if the position info was not added to the TT
 */
bool tt_add(const uint64_t position_hash, const struct move mv, const uint8_t depth, const int32_t score,
            const enum node_type node_type) {
    assert(validate_move(mv));
    assert(validate_node_type(node_type));
    assert(tt != NULL);
    assert(num_tt_elems > 0);
    assert(depth <= MAX_SEARCH_DEPTH);

    const uint32_t hash_idx = get_index(position_hash);
    struct tt_entry *entry = &tt[hash_idx];

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
    entry->score = score;
    entry->node_type = node_type;
    return true;
}

/**
 * @brief Checks to see if a given Position hash is present in the TT, and returns the associated move is it is
 * 
 * @param position_hash The Position Hash
 * @param mv The move, populated with the move if return value is true
 * @return true if the hash is present
 * @return false if the has is not present
 */
bool tt_probe_position(const uint64_t position_hash, struct move *mv) {
    const uint32_t hash_idx = get_index(position_hash);

    const struct tt_entry *elem = &tt[hash_idx];

    if (elem->position_hash == position_hash) {
        *mv = elem->mv;
        return true;
    }
    return false;
}

/**
 * @brief Disposes of the TT
 * 
 */
void tt_dispose(void) {
    if (tt != NULL) {
        free(tt);

        tt = NULL;
        num_tt_elems = 0;
    }
}

static void set_tt_size(uint64_t size_in_bytes) {
    num_tt_elems = (uint32_t)(size_in_bytes / sizeof(struct tt_entry));
    REQUIRE(num_tt_elems > MIN_NUM_TT_SLOTS, "Insufficient number of TT slots");

    tt = (struct tt_entry *)calloc(num_tt_elems, sizeof(struct tt_entry));
}

static void init_table(void) {
    for (uint64_t i = 0; i < num_tt_elems; i++) {
        struct tt_entry *entry = &tt[i];
        entry->slot_used = false;
    }
}

static uint32_t get_index(const uint64_t hash) {
    return (uint32_t)(hash % num_tt_elems);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static bool validate_node_type(const enum node_type nt) {
    switch (nt) {
    case NODE_EXACT:
        return true;
    case NODE_ALPHA:
        return true;
    case NODE_BETA:
        return true;
    default:
        return false;
    }
}
#pragma GCC diagnostic pop

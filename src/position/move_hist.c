/*  MIT License
 *
 *  Copyright (c) 2019 Eddie McNally
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

#include "move_hist.h"
#include "castle_perms.h"
#include "move.h"
#include "position.h"
#include <assert.h>
#include <stdio.h>

struct move_state {
    // the move being made
    struct move mv;
    // active/inactive en passant
    struct en_pass_active en_passant;
    //position hash
    uint64_t hashkey;
    // active castle permissions
    struct cast_perm_container castle_perm_container;
    // current 50-rule counter
    uint8_t fifty_move_counter;
};

struct move_hist {
    // move history
    struct move_state history[MAX_GAME_MOVES];
    struct move_state *free_slot;
    uint16_t num_used_slots;
};

static bool compare_move_states(struct move_state *ms1, struct move_state *ms2);

/**
 * @brief       Initialises the move history environment
 *
 * @return      Pointer to initialised struct
 */
struct move_hist *move_hist_init(void) {
    struct move_hist *retval =
        (struct move_hist *)calloc(MAX_GAME_MOVES, sizeof(struct move_hist));

    retval->free_slot = &retval->history[0];
    retval->num_used_slots = 0;
    return retval;
}

void move_hist_push(struct move_hist *move_history, const struct move mv,
                    const uint8_t fifty_move_counter,
                    const struct en_pass_active en_passant,
                    const uint64_t hashkey,
                    const struct cast_perm_container castle_perm_cont) {

    assert(move_history->free_slot <= &move_history->history[MAX_GAME_MOVES]);
    assert(move_history->num_used_slots < MAX_GAME_MOVES);

    move_history->free_slot->mv = mv;
    move_history->free_slot->fifty_move_counter = fifty_move_counter;
    move_history->free_slot->en_passant = en_passant;
    move_history->free_slot->hashkey = hashkey;
    move_history->free_slot->castle_perm_container = castle_perm_cont;

    move_history->num_used_slots++;
    move_history->free_slot++;
}

void move_hist_pop(struct move_hist *move_history, struct move *mv,
                   uint8_t *fifty_move_counter,
                   struct en_pass_active *en_passant, uint64_t *hashkey,
                   struct cast_perm_container *castle_perm_container) {

    assert(move_history->free_slot > &move_history->history[0]);
    assert(move_history->num_used_slots > 0);

    *mv = move_history->free_slot->mv;
    *fifty_move_counter = move_history->free_slot->fifty_move_counter;
    *en_passant = move_history->free_slot->en_passant;
    *hashkey = move_history->free_slot->hashkey;
    *castle_perm_container = move_history->free_slot->castle_perm_container;

    move_history->num_used_slots--;
    move_history->free_slot--;
}

bool move_hist_compare(struct move_hist *hist1, struct move_hist *hist2) {

    if (hist1->num_used_slots != hist2->num_used_slots) {
        return false;
    }

    struct move_state *ms1 = &hist1->history[0];
    struct move_state *ms2 = &hist2->history[0];
    uint16_t num_slots = hist1->num_used_slots;

    for (int i = 0; i < num_slots; i++) {
        bool same_move_state = compare_move_states(ms1, ms2);
        if (same_move_state == false) {
            return false;
        }
        ms1++;
        ms2++;
    }
    return true;
}

static bool compare_move_states(struct move_state *ms1,
                                struct move_state *ms2) {
    if (move_compare(ms1->mv, ms2->mv) == false) {
        return false;
    }

    if (ms1->en_passant.sq != ms2->en_passant.sq) {
        return false;
    }

    if (ms1->en_passant.is_active != ms2->en_passant.is_active) {
        return false;
    }

    if (ms1->hashkey != ms2->hashkey) {
        return false;
    }

    if (ms1->fifty_move_counter != ms2->fifty_move_counter) {
        return false;
    }

    bool same_cast_perms = cast_compare_perms(ms1->castle_perm_container,
                                              ms2->castle_perm_container);
    if (same_cast_perms == false) {
        return false;
    }

    return true;
}

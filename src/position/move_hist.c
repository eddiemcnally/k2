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

static uint64_t INIT_TOKEN = 0xdeadbeefc0c0face;

struct move_hist {
    uint64_t init_key;
    // move history
    struct move_state history[MAX_GAME_MOVES];
    struct move_state *free_slot;
    uint16_t num_used_slots;
};

static bool compare_move_states(const struct move_state *ms1,
                                const struct move_state *ms2);
static bool validate_move_history(const struct move_hist *mh);
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
    retval->init_key = INIT_TOKEN;

    return retval;
}

/**
 * @brief       Frees up any memory associated with the move history
 *
 * @param       Pointer to move history struct
 */
void move_hist_release_memory(struct move_hist *mh) {
    assert(validate_move_history(mh));

    mh->init_key = 0;
    free(mh);
}

/**
 * @brief       pushes the position state onto the stack
 *
 * @param       Pointer to move history struct
 * @param       The move about to be made
 * @param       The 50-move counter
 * @param       The en passant state
 * @param       The current position hash key
 * @param       The current castle permissions
 */
void move_hist_push(struct move_hist *move_history, const struct move mv,
                    const uint8_t fifty_move_counter,
                    const struct en_pass_active en_passant,
                    const uint64_t hashkey,
                    const struct cast_perm_container castle_perm_cont) {

    assert(validate_move_history(move_history));

    move_history->free_slot->mv = mv;
    move_history->free_slot->fifty_move_counter = fifty_move_counter;
    move_history->free_slot->en_passant = en_passant;
    move_history->free_slot->hashkey = hashkey;
    move_history->free_slot->castle_perm_container = castle_perm_cont;

    move_history->num_used_slots++;
    move_history->free_slot++;
}

/**
 * @brief       Pop the position info off the stack
 *
 * @param       Pointer to move history struct
 * @param       The move about to be made
 * @param       The 50-move counter
 * @param       The en passant state
 * @param       The current position hash key
 * @param       The current castle permissions
 */
void move_hist_pop(struct move_hist *move_history, struct move *mv,
                   uint8_t *fifty_move_counter,
                   struct en_pass_active *en_passant, uint64_t *hashkey,
                   struct cast_perm_container *castle_perm_container) {

    assert(validate_move_history(move_history));

    *mv = move_history->free_slot->mv;
    *fifty_move_counter = move_history->free_slot->fifty_move_counter;
    *en_passant = move_history->free_slot->en_passant;
    *hashkey = move_history->free_slot->hashkey;
    *castle_perm_container = move_history->free_slot->castle_perm_container;

    move_history->num_used_slots--;
    move_history->free_slot--;
}

/**
 * @brief   Returns the number of used stack slots
 * @param   Pointer to the move hist pory struct
 */
uint16_t move_hist_get_num(const struct move_hist *mh) {
    return mh->num_used_slots;
}

/**
 * @brief   Compares 2 move histories for equality
 * @param   The 1st move history
 * @param   The 2nd move history
 * @return  true is the same, false otherwise
 */
bool move_hist_compare(const struct move_hist *hist1,
                       const struct move_hist *hist2) {

    assert(validate_move_history(hist1));
    assert(validate_move_history(hist2));

    if (hist1->num_used_slots != hist2->num_used_slots) {
        return false;
    }

    const struct move_state *ms1 = &hist1->history[0];
    const struct move_state *ms2 = &hist2->history[0];
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

static bool compare_move_states(const struct move_state *ms1,
                                const struct move_state *ms2) {
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

static bool validate_move_history(const struct move_hist *mh) {
    if (mh->init_key != INIT_TOKEN) {
        return false;
    }

    if (mh->num_used_slots > MAX_GAME_MOVES) {
        return false;
    }

    if (mh->free_slot < &mh->history[0]) {
        return false;
    }

    if (mh->free_slot > &mh->history[MAX_GAME_MOVES]) {
        return false;
    }

    return true;
}

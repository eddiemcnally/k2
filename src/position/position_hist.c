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

#include "position_hist.h"
#include "board.h"
#include "castle_perms.h"
#include "move.h"
#include "position.h"
#include <assert.h>
#include <stdio.h>

struct move_state {
    //position hash
    uint64_t hashkey;
    // storage for cloned board
    uint8_t board[BOARD_SIZE_BYTES];
    // the move being made
    struct move mv;
    // active/inactive en passant
    struct en_pass_active en_passant;
    // side to move
    enum colour side;
    // active castle permissions
    struct cast_perm_container castle_perm_container;
    // current 50-rule counter
    uint8_t fifty_move_counter;
};

static uint64_t INIT_TOKEN = 0xdeadbeefc0c0face;

struct position_hist {
    uint64_t init_key;
    // move history
    struct move_state history[MAX_GAME_MOVES];
    struct move_state *free_slot;
    uint16_t num_used_slots;
};

static bool compare_move_states(const struct move_state *ms1,
                                const struct move_state *ms2);
static bool validate_move_history(const struct position_hist *mh);
/**
 * @brief       Initialises the move history environment
 *
 * @return      Pointer to initialised struct
 */
struct position_hist *position_hist_init(void) {
    struct position_hist *retval = (struct position_hist *)calloc(
        MAX_GAME_MOVES, sizeof(struct position_hist));

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
void position_hist_release_memory(struct position_hist *mh) {
    assert(validate_move_history(mh));

    mh->init_key = 0;
    free(mh);
}

void position_hist_push(struct position_hist *pos_history, const struct move mv,
                        const uint8_t fifty_move_counter,
                        const struct en_pass_active en_passant,
                        const uint64_t hashkey,
                        const struct cast_perm_container castle_perm_cont,
                        const struct board *brd) {

    assert(validate_move_history(pos_history));
    assert(validate_board(brd));

    pos_history->free_slot->mv = mv;
    pos_history->free_slot->fifty_move_counter = fifty_move_counter;
    pos_history->free_slot->en_passant = en_passant;
    pos_history->free_slot->hashkey = hashkey;
    pos_history->free_slot->castle_perm_container = castle_perm_cont;
    brd_clone(brd, (struct board *)(&pos_history->free_slot->board));

    assert(validate_board((struct board *)(&pos_history->free_slot->board)));

    pos_history->num_used_slots++;
    pos_history->free_slot++;
}

void position_hist_pop(struct position_hist *pos_history, struct move *mv,
                       uint8_t *fifty_move_counter,
                       struct en_pass_active *en_passant, uint64_t *hashkey,
                       struct cast_perm_container *castle_perm_container,
                       struct board *brd) {

    pos_history->num_used_slots--;
    pos_history->free_slot--;

    assert(validate_move_history(pos_history));

    *mv = pos_history->free_slot->mv;
    *fifty_move_counter = pos_history->free_slot->fifty_move_counter;
    *en_passant = pos_history->free_slot->en_passant;
    *hashkey = pos_history->free_slot->hashkey;
    *castle_perm_container = pos_history->free_slot->castle_perm_container;

    assert(validate_board((struct board *)(&pos_history->free_slot->board)));

    brd_clone((struct board *)(&pos_history->free_slot->board), brd);
}

/**
 * @brief   Returns the number of used stack slots
 * @param   Pointer to the move hist pory struct
 */
uint16_t position_hist_get_num(const struct position_hist *mh) {
    return mh->num_used_slots;
}

/**
 * @brief   Compares 2 move histories for equality
 * @param   The 1st move history
 * @param   The 2nd move history
 * @return  true is the same, false otherwise
 */
bool position_hist_compare(const struct position_hist *hist1,
                           const struct position_hist *hist2) {

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

static bool validate_move_history(const struct position_hist *mh) {
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

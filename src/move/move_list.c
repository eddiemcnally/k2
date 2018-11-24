/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "move_list.h"
#include "move.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const uint16_t MOVE_LIST_INIT_KEY = 0xdead;

#define MOVE_LIST_MAX_LEN 2048

struct move_list {
    uint16_t struct_init_key;
    uint16_t move_count;
    struct move move_list[MOVE_LIST_MAX_LEN];
};

// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief       Allocates and initialises a new move list
 *
 * @return      A ptr to a new move_list instance
 */
struct move_list* mvl_allocate(void)
{
    struct move_list* retval = (struct move_list*)malloc(sizeof(struct move_list));
    memset(retval, 0, sizeof(struct move_list));
    retval->struct_init_key = MOVE_LIST_INIT_KEY;

    return retval;
}

/**
 * @brief       Deallocates a move_list and returns resources
 *
 * @param mvl   A ptr to the move_list to free up.
 */
void mvl_deallocate(struct move_list* mvl)
{
    assert(validate_move_list(mvl));
    memset(mvl, 0, sizeof(struct move_list));
    free(mvl);
}

/**
 * @brief       Extract move count from move list
 *
 * @param mvl   Pointer to move_list
 * @return              Number of moves in the move list
 */
uint16_t mvl_get_move_count(const struct move_list* mvl)
{
    assert(validate_move_list(mvl));
    return mvl->move_count;
}

/**
 * @brief       Returns the move at the given place in the move list
 *
 * @param mvl   The move list
 * @param offset The move offset in the move list
 * @return      The move at the move list offset
 */
struct move mvl_get_move_at_offset(const struct move_list* mvl, uint16_t offset)
{
    assert(validate_move_list(mvl));
    assert(offset <= mvl->move_count - 1);

    return mvl->move_list[offset];
}

/**
 * @brief       Appends a move to the end of the current move_list
 *
 * @param mvl   The move_list instance
 * @param mv    The move to add
 */
void mvl_add(struct move_list* mvl, const struct move mv)
{
    assert(validate_move_list(mvl));
    assert(validate_move(mv));

    mvl->move_list[mvl->move_count] = mv;
    mvl->move_count++;
}

/**
 * @brief       Resets the move list to the initial empty state
 *
 * @param mvl   Pointer to the move list
 */
void mvl_reset(struct move_list* mvl)
{
    assert(validate_move_list(mvl));

    mvl->move_count = 0;
}

/**
 * @brief       Tests to see if a move is contained within the move list
 *
 * @param mvl   Pointer to the move list
 * @param mv    The move to search for
 * @return true if move is in list, false otherwise
 */
bool mvl_contains_move(const struct move_list* mvl, const struct move mv)
{
    assert(validate_move_list(mvl));
    assert(validate_move(mv));

    for (int i = 0; i < mvl->move_count; i++) {
        if (move_compare(mvl->move_list[i], mv)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief       Prints out the moves in a move list
 *
 * @param mvl   Pointer to the move list
 */
void mvl_print(const struct move_list* mvl)
{
    assert(validate_move_list(mvl));

    uint16_t move_count = mvl_get_move_count(mvl);

    for (uint16_t i = 0; i < move_count; i++) {
        struct move m = mvl_get_move_at_offset(mvl, i);
        printf("%s\n", move_print(m));
    }
}

/**
 * @brief       Validates move list and contained moves
 * @param mvl   Pointer to the move list
 */
bool validate_move_list(const struct move_list* mvl)
{
    if (mvl->struct_init_key != MOVE_LIST_INIT_KEY) {
        assert(false);
    }

    if (mvl->move_count >= MOVE_LIST_MAX_LEN) {
        assert(false);
    }

    return true;
}

/**
 * @brief       Returns ths max allowed size of the move list
 *
 * @return      The max size
 */
uint16_t mvl_get_mvl_max_size(void)
{
    return MOVE_LIST_MAX_LEN;
}

/**
 * @brief               Compares 2 move lists for equivalency
 * @param first         Pointer to the first move list
 * @param second        Pointer to the second move list
 * @return              True if the lists are the same, false otherwise
 */
bool mvl_compare(const struct move_list* first, const struct move_list* second)
{
    assert(validate_move_list(first));
    assert(validate_move_list(second));

    if (first->move_count != second->move_count) {
        return false;
    }

    for (int i = 0; i < first->move_count; i++) {
        if (move_compare(first->move_list[i], second->move_list[i]) == false) {
            return false;
        }
    }
    return true;
}

// ==================================================================
//
// private functions
//
// ==================================================================

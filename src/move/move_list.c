// Copyright (c) 2017 Eddie McNally
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "move_list.h"
#include "move.h"


#define     MOVE_LIST_INIT_KEY      ((uint16_t)0xc0c0babe)



struct move_list
{
	uint16_t 	struct_init_key;
	uint16_t 	move_count;
	move_t 		move_list[MOVE_LIST_MAX_LEN];
};

static void validate_move_list(const struct move_list *mvl);
static void validate_move_list_size(const struct move_list *mvl);



// ==================================================================
//
// public functions
//
// ==================================================================



/**
 * @brief Allocate a new move_list
 * @details Allocates and initialises a new move list
 *
 * @return A ptr to a new move_list instance
 */
struct move_list* mvl_allocate(void)
{
	struct move_list *retval = (struct move_list *)malloc(sizeof(struct move_list));
	memset(retval, 0, sizeof(struct move_list));
	retval->struct_init_key = MOVE_LIST_INIT_KEY;

	return retval;
}

/**
 * @brief Deallocate a move_list instance
 * @details Deallocates a move_list and returns resources
 *
 * @param move_list A ptr to the move_list to free up.
 */
void mvl_deallocate(struct move_list *mvl)
{
	validate_move_list(mvl);
	memset(mvl, 0, sizeof (struct move_list));
	free(mvl);
}

/**
 * @brief       Extract move count from move list
 * @details
 *
 * @param move_list Pointer to move_list
 * @return Number of moves in the move list
 */
uint16_t mvl_get_move_count(const struct move_list *mvl)
{
	validate_move_list(mvl);
	return mvl->move_count;
}

/**
 * @brief 		Extract the move array
 * @details 	Extracts the move array from the move_list
 *
 * @param move_list The move list
 * @return A pointer to an array of move_t types
 */
move_t* mvl_get_move_array(struct move_list *mvl)
{
	validate_move_list(mvl);
	return mvl->move_list;
}

/**
 * @brief 		Append a move
 * @details 	Appends a move to the end of the current move_list
 *
 * @param move_list The move_list instance
 * @param mv The move to add
 */
void mvl_add(struct move_list *mvl, move_t mv)
{
	validate_move_list(mvl);
	validate_move_list_size(mvl);

	mvl->move_list[mvl->move_count] = mv;
	mvl->move_count++;
}

/**
 * @brief Resets move list to init state
 * @details Resets the move list to the initial empty state
 *
 * @param move_list Pointer to the move list
 */
void mvl_reset(struct move_list *mvl)
{
	validate_move_list(mvl);
	validate_move_list_size(mvl);

	mvl->move_count = 0;
}

// ==================================================================
//
// private functions
//
// ==================================================================




static void validate_move_list(const struct move_list *mvl)
{
	assert(mvl->struct_init_key == MOVE_LIST_INIT_KEY);
}

static void validate_move_list_size(const struct move_list *mvl)
{
	assert(mvl->move_count < MOVE_LIST_MAX_LEN - 1);
}

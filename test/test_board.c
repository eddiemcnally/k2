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


#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "board.h"
#include "piece.h"
#include "square.h"
#include "test_board.h"

static enum piece pce_list[NUM_PIECES] = {WPAWN, WBISHOP, WKNIGHT, WROOK, WQUEEN, WKING,
                                          BPAWN, BBISHOP, BKNIGHT, BROOK, BQUEEN, BKING
                                         };
static enum piece white_pce_list[NUM_PIECE_TYPES] = {WPAWN, WBISHOP, WKNIGHT, WROOK, WQUEEN, WKING};
static enum piece black_pce_list[NUM_PIECE_TYPES] = {BPAWN, BBISHOP, BKNIGHT, BROOK, BQUEEN, BKING};


static bool is_sq_in_list(const enum square * sqlist, const int list_size, const enum square sq_to_check);



void test_board_brd_allocate_deallocate(void **state)
{
	struct board *brd = brd_allocate();
	brd_validate(brd);
	brd_deallocate(brd);
}


void test_board_brd_bulk_add_remove_piece(void **state)
{
	struct board *brd = brd_allocate();

	for (int i = 0; i < NUM_PIECES; i++)
	{
		for (enum square sq = a1; sq <= h8; sq++)
		{
			enum piece pce = pce_list[i];

			// add piece
			brd_add_piece(brd, pce, sq);

			// verify it's there
			enum piece found_pce;
			bool found = brd_try_get_piece_on_square(brd, sq, &found_pce);
			assert_true(found);
			assert_true(found_pce == pce);
			bool is_occupied = brd_is_sq_occupied(brd, sq);
			assert_true(is_occupied);

			// remove piece
			brd_remove_piece(brd, pce, sq);

			// verify it's gone
			found = brd_try_get_piece_on_square(brd, sq, &found_pce);
			assert_false(found);
			is_occupied = brd_is_sq_occupied(brd, sq);
			assert_false(is_occupied);
		}
	}
	brd_deallocate(brd);
}



void test_board_brd_move_piece(void **state)
{
	struct board *brd = brd_allocate();

	for (int i = 0; i < NUM_PIECES; i++)
	{
		enum piece pce = pce_list[i];
		for (enum square from_sq = a1; from_sq <= h8; from_sq++)
		{
			for (enum square to_sq = a1; to_sq <= h8; to_sq++)
			{
				if (from_sq == to_sq)
				{
					continue;
				}
				// add piece
				brd_add_piece(brd, pce, from_sq);

				// verify it's there
				enum piece found_pce;
				bool found = brd_try_get_piece_on_square(brd, from_sq, &found_pce);
				assert_true(found);
				assert_true(found_pce == pce);
				bool is_occupied = brd_is_sq_occupied(brd, from_sq);
				assert_true(is_occupied);

				// move it
				brd_move_piece(brd, pce, from_sq, to_sq);

				// verify it's not on the from_sq
				found = brd_try_get_piece_on_square(brd, from_sq, &found_pce);
				assert_false(found);
				is_occupied = brd_is_sq_occupied(brd, from_sq);
				assert_false(is_occupied);
				// verify it's now on the to_sq
				found = brd_try_get_piece_on_square(brd, to_sq, &found_pce);
				assert_true(found);
				is_occupied = brd_is_sq_occupied(brd, to_sq);
				assert_true(is_occupied);

				// remove piece
				brd_remove_piece(brd, pce, to_sq);
			}
		}
	}

	brd_deallocate(brd);
}


void test_board_brd_get_piece_bb(void **state)
{
#define NUM_TEST_SQ 10

	enum square sq_list[NUM_TEST_SQ] = {a1, b3, c1, h8, d4, h8, a8, h1, g7, f2};
	struct board *brd = brd_allocate();

	for (int i = 0; i < NUM_PIECES; i++)
	{
		enum piece pce = pce_list[i];
		// add piece to all test squares
		for (int j = 0; j < NUM_TEST_SQ; j++)
		{
			enum square sq = sq_list[j];
			brd_add_piece(brd, pce, sq);
		}

		bitboard_t bb = brd_get_piece_bb(brd, pce);

		// verify all bits (squares) are set
		for (int j = 0; j < NUM_TEST_SQ; j++)
		{
			enum square sq = sq_list[j];
			bool is_set = bb_is_set(bb, sq);
			assert_true(is_set);
		}

		// now verify that all other squares are *not* set
		for (enum square sq = a1; sq <= h8; sq++)
		{
			if (is_sq_in_list(sq_list, NUM_TEST_SQ, sq))
			{
				// ignore square
				continue;
			}
			bool is_set = bb_is_set(bb, sq);
			assert_false(is_set);
		}
	}
	brd_deallocate(brd);
}



void test_board_brd_get_colour_bb_black(void **state)
{
#define NUM_TEST_SQ 10

	enum square sq_list[NUM_TEST_SQ] = {a1, b3, c1, h8, d4, h8, a8, h1, g7, f2};
	struct board *brd = brd_allocate();

	for (int i = 0; i < NUM_PIECE_TYPES; i++)
	{
		enum piece pce = black_pce_list[i];
		// add piece to all test squares
		for (int j = 0; j < NUM_TEST_SQ; j++)
		{
			enum square sq = sq_list[j];
			brd_add_piece(brd, pce, sq);
		}

		bitboard_t bb = brd_get_colour_bb(brd, BLACK);

		// verify all bits (squares) are set
		for (int j = 0; j < NUM_TEST_SQ; j++)
		{
			enum square sq = sq_list[j];
			bool is_set = bb_is_set(bb, sq);
			assert_true(is_set);
		}

		// now verify that all other squares are *not* set
		for (enum square sq = a1; sq <= h8; sq++)
		{
			if (is_sq_in_list(sq_list, NUM_TEST_SQ, sq))
			{
				// ignore square
				continue;
			}
			bool is_set = bb_is_set(bb, sq);
			assert_false(is_set);
		}
	}
	brd_deallocate(brd);
}


void test_board_brd_get_colour_bb_white(void **state)
{
#define NUM_TEST_SQ 10

	enum square sq_list[NUM_TEST_SQ] = {a1, b3, c1, h8, d4, h8, a8, h1, g7, f2};
	struct board *brd = brd_allocate();

	for (int i = 0; i < NUM_PIECE_TYPES; i++)
	{
		enum piece pce = white_pce_list[i];
		// add piece to all test squares
		for (int j = 0; j < NUM_TEST_SQ; j++)
		{
			enum square sq = sq_list[j];
			brd_add_piece(brd, pce, sq);
		}

		bitboard_t bb = brd_get_colour_bb(brd, WHITE);

		// verify all bits (squares) are set
		for (int j = 0; j < NUM_TEST_SQ; j++)
		{
			enum square sq = sq_list[j];
			bool is_set = bb_is_set(bb, sq);
			assert_true(is_set);
		}

		// now verify that all other squares are *not* set
		for (enum square sq = a1; sq <= h8; sq++)
		{
			if (is_sq_in_list(sq_list, NUM_TEST_SQ, sq))
			{
				// ignore square
				continue;
			}
			bool is_set = bb_is_set(bb, sq);
			assert_false(is_set);
		}
	}
	brd_deallocate(brd);
}



static bool is_sq_in_list(const enum square * sqlist, const int list_size, const enum square sq_to_check)
{
	for (int i = 0; i < list_size; i++)
	{
		if (*(sqlist + i) == sq_to_check)
		{
			return true;
		}
	}
	return false;
}

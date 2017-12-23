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


/*

-- To test

bool brd_is_sq_occupied(const struct board* brd, const enum square sq);
bool brd_try_get_piece_on_square(const struct board* brd, const enum square sq, enum piece *pce);
void brd_add_piece(struct board* brd, const enum piece pce, const enum square sq);
bitboard_t brd_get_piece_bb(const struct board* brd, const enum piece pce);
void brd_remove_piece(struct board* brd, const enum piece pce, const enum square sq);
void brd_move_piece(struct board* brd, const enum piece pce, const enum square from_sq, const enum square to_sq);
bitboard_t brd_get_colour_bb(const struct board* brd, const enum colour colour);

*/


void test_board_brd_allocate_deallocate(void **state)
{
	struct board *brd = brd_allocate();
	brd_validate(brd);
	brd_deallocate(brd);
}


void test_board_brd_bulk_add_remove_piece(void **state)
{
	//int mark[5] = {19, 10, 8, 17, 9};
	enum piece pce_list[NUM_PIECES] = {WPAWN, WBISHOP, WKNIGHT, WROOK, WQUEEN, WKING,
	                                   BPAWN, BBISHOP, BKNIGHT, BROOK, BQUEEN, BKING
	                                  };

	struct board *brd = brd_allocate();

	for (int i = 0; i < NUM_PIECES; i++)
	{
		for (enum square sq = a1; sq <= h8; sq++)
		{
			enum piece pce = pce_list[i];

			brd_add_piece(brd, pce, sq);

			enum piece found_pce;
			bool found = brd_try_get_piece_on_square(brd, sq, &found_pce);
			assert_true(found);
			assert_true(found_pce == pce);

			bool is_occupied = brd_is_sq_occupied(brd, sq);
			assert_true(is_occupied);

			brd_remove_piece(brd, pce, sq);
			found = brd_try_get_piece_on_square(brd, sq, &found_pce);
			assert_false(found);

			is_occupied = brd_is_sq_occupied(brd, sq);
			assert_false(is_occupied);
		}
	}
}




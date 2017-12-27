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
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "position.h"
#include "test_position.h"



void test_position_add_castle_perm(void **state)
{
	uint8_t perm = CAST_PERM_NONE;

	add_cast_perm(&perm, CAST_PERM_WK);
	bool b = has_cast_perm(perm, CAST_PERM_WK);
	assert_true(b);

	b = has_cast_perm(perm, CAST_PERM_WQ);
	assert_false(b);
	b = has_cast_perm(perm, CAST_PERM_BK);
	assert_false(b);
	b = has_cast_perm(perm, CAST_PERM_BK);
	assert_false(b);

	add_cast_perm(&perm, CAST_PERM_WQ);
	b = has_cast_perm(perm, CAST_PERM_WQ);
	assert_true(b);

	add_cast_perm(&perm, CAST_PERM_BQ);
	b = has_cast_perm(perm, CAST_PERM_BQ);
	assert_true(b);

	add_cast_perm(&perm, CAST_PERM_BK);
	b = has_cast_perm(perm, CAST_PERM_BK);
	assert_true(b);

	// now check all
	b = has_cast_perm(perm, CAST_PERM_WK);
	assert_true(b);
	b = has_cast_perm(perm, CAST_PERM_WQ);
	assert_true(b);
	b = has_cast_perm(perm, CAST_PERM_BK);
	assert_true(b);
	b = has_cast_perm(perm, CAST_PERM_BQ);
	assert_true(b);
}

void test_position_remove_castle_perm(void **state)
{
	uint8_t perm = CAST_PERM_NONE;

	add_cast_perm(&perm, CAST_PERM_WK);
	add_cast_perm(&perm, CAST_PERM_WQ);
	add_cast_perm(&perm, CAST_PERM_BK);
	add_cast_perm(&perm, CAST_PERM_BQ);

	// check all as expected
	bool b = has_cast_perm(perm, CAST_PERM_WK);
	assert_true(b);
	b = has_cast_perm(perm, CAST_PERM_WQ);
	assert_true(b);
	b = has_cast_perm(perm, CAST_PERM_BK);
	assert_true(b);
	b = has_cast_perm(perm, CAST_PERM_BQ);
	assert_true(b);


	remove_cast_perm(&perm, CAST_PERM_WK);
	b = has_cast_perm(perm, CAST_PERM_WK);
	assert_false(b);

	remove_cast_perm(&perm, CAST_PERM_WQ);
	b = has_cast_perm(perm, CAST_PERM_WQ);
	assert_false(b);

	remove_cast_perm(&perm, CAST_PERM_BQ);
	b = has_cast_perm(perm, CAST_PERM_BQ);
	assert_false(b);

	remove_cast_perm(&perm, CAST_PERM_BK);
	b = has_cast_perm(perm, CAST_PERM_BK);
	assert_false(b);
}



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



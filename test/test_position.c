#include <stdbool.h>
#include <stdint.h>
#include "seatest.h"
#include "position.h"
#include "test_position.h"

static void test_add_castle_perm(void);
static void test_remove_castle_perm(void);



void test_fixture_position(void)
{
	test_fixture_start();               // starts a fixture

	run_test(test_add_castle_perm);
	run_test(test_remove_castle_perm);

	test_fixture_end();                 // ends a fixture
}


static void test_add_castle_perm(void)
{
	cast_perm_t perm = CAST_PERM_NONE;

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

static void test_remove_castle_perm(void)
{
	cast_perm_t perm = CAST_PERM_NONE;

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



#include "seatest.h"
#include "test_piece.h"
#include "test_square.h"
#include "test_fen.h"


static void all_tests(void);
static void k2_suite_setup(void);
static void k2_suite_teardown(void);


static void all_tests(void)
{
	test_fixture_piece();
	test_fixture_square();
	test_fixture_fen();
}

static void k2_suite_setup(void) {}
static void k2_suite_teardown(void) {}

int main(int argc, char** argv)
{
	return seatest_testrunner(argc, argv, all_tests, k2_suite_setup, k2_suite_teardown);
}
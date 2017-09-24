#include "seatest.h"
#include "test_piece.h"
#include "test_square.h"


void all_tests( void )
{
	test_fixture_piece();
	test_fixture_square();
}

void k2_suite_setup( void )
{}

void k2_suite_teardown( void )
{}

int main( int argc, char** argv )
{
	return seatest_testrunner(argc, argv, all_tests, k2_suite_setup, k2_suite_teardown);
}
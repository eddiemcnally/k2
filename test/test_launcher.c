#include "seatest.h"
#include "test_piece.h"


void all_tests( void )
{
	test_fixture_piece();
}

void k2_suite_setup( void )
{}

void k2_suite_teardown( void )
{}

int main( int argc, char** argv )
{
	return seatest_testrunner(argc, argv, all_tests, k2_suite_setup, k2_suite_teardown);
}
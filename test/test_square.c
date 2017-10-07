#include "seatest.h"
#include "square.h"
#include "test_square.h"



static void test_get_rank(void);
static void test_get_file(void);
static void test_get_square_from_rank_file(void);


void test_fixture_square(void)
{
	test_fixture_start();               // starts a fixture

	run_test(test_get_file);
	run_test(test_get_rank);
	run_test(test_get_square_from_rank_file);

	test_fixture_end();                 // ends a fixture
}


static void test_get_rank(void)
{
	assert_true(get_rank(a1) == RANK_1);
	assert_true(get_rank(b1) == RANK_1);
	assert_true(get_rank(c1) == RANK_1);
	assert_true(get_rank(d1) == RANK_1);
	assert_true(get_rank(e1) == RANK_1);
	assert_true(get_rank(f1) == RANK_1);
	assert_true(get_rank(g1) == RANK_1);
	assert_true(get_rank(h1) == RANK_1);

	assert_true(get_rank(a2) == RANK_2);
	assert_true(get_rank(b2) == RANK_2);
	assert_true(get_rank(c2) == RANK_2);
	assert_true(get_rank(d2) == RANK_2);
	assert_true(get_rank(e2) == RANK_2);
	assert_true(get_rank(f2) == RANK_2);
	assert_true(get_rank(g2) == RANK_2);
	assert_true(get_rank(h2) == RANK_2);

	assert_true(get_rank(a3) == RANK_3);
	assert_true(get_rank(b3) == RANK_3);
	assert_true(get_rank(c3) == RANK_3);
	assert_true(get_rank(d3) == RANK_3);
	assert_true(get_rank(e3) == RANK_3);
	assert_true(get_rank(f3) == RANK_3);
	assert_true(get_rank(g3) == RANK_3);
	assert_true(get_rank(h3) == RANK_3);

	assert_true(get_rank(a4) == RANK_4);
	assert_true(get_rank(b4) == RANK_4);
	assert_true(get_rank(c4) == RANK_4);
	assert_true(get_rank(d4) == RANK_4);
	assert_true(get_rank(e4) == RANK_4);
	assert_true(get_rank(f4) == RANK_4);
	assert_true(get_rank(g4) == RANK_4);
	assert_true(get_rank(h4) == RANK_4);

	assert_true(get_rank(a5) == RANK_5);
	assert_true(get_rank(b5) == RANK_5);
	assert_true(get_rank(c5) == RANK_5);
	assert_true(get_rank(d5) == RANK_5);
	assert_true(get_rank(e5) == RANK_5);
	assert_true(get_rank(f5) == RANK_5);
	assert_true(get_rank(g5) == RANK_5);
	assert_true(get_rank(h5) == RANK_5);

	assert_true(get_rank(a6) == RANK_6);
	assert_true(get_rank(b6) == RANK_6);
	assert_true(get_rank(c6) == RANK_6);
	assert_true(get_rank(d6) == RANK_6);
	assert_true(get_rank(e6) == RANK_6);
	assert_true(get_rank(f6) == RANK_6);
	assert_true(get_rank(g6) == RANK_6);
	assert_true(get_rank(h6) == RANK_6);

	assert_true(get_rank(a7) == RANK_7);
	assert_true(get_rank(b7) == RANK_7);
	assert_true(get_rank(c7) == RANK_7);
	assert_true(get_rank(d7) == RANK_7);
	assert_true(get_rank(e7) == RANK_7);
	assert_true(get_rank(f7) == RANK_7);
	assert_true(get_rank(g7) == RANK_7);
	assert_true(get_rank(h7) == RANK_7);

	assert_true(get_rank(a8) == RANK_8);
	assert_true(get_rank(b8) == RANK_8);
	assert_true(get_rank(c8) == RANK_8);
	assert_true(get_rank(d8) == RANK_8);
	assert_true(get_rank(e8) == RANK_8);
	assert_true(get_rank(f8) == RANK_8);
	assert_true(get_rank(g8) == RANK_8);
	assert_true(get_rank(h8) == RANK_8);
}


static void test_get_file(void)
{
	assert_true(get_file(a1) == FILE_A);
	assert_true(get_file(b1) == FILE_B);
	assert_true(get_file(c1) == FILE_C);
	assert_true(get_file(d1) == FILE_D);
	assert_true(get_file(e1) == FILE_E);
	assert_true(get_file(f1) == FILE_F);
	assert_true(get_file(g1) == FILE_G);
	assert_true(get_file(h1) == FILE_H);

	assert_true(get_file(a2) == FILE_A);
	assert_true(get_file(b2) == FILE_B);
	assert_true(get_file(c2) == FILE_C);
	assert_true(get_file(d2) == FILE_D);
	assert_true(get_file(e2) == FILE_E);
	assert_true(get_file(f2) == FILE_F);
	assert_true(get_file(g2) == FILE_G);
	assert_true(get_file(h2) == FILE_H);

	assert_true(get_file(a3) == FILE_A);
	assert_true(get_file(b3) == FILE_B);
	assert_true(get_file(c3) == FILE_C);
	assert_true(get_file(d3) == FILE_D);
	assert_true(get_file(e3) == FILE_E);
	assert_true(get_file(f3) == FILE_F);
	assert_true(get_file(g3) == FILE_G);
	assert_true(get_file(h3) == FILE_H);

	assert_true(get_file(a4) == FILE_A);
	assert_true(get_file(b4) == FILE_B);
	assert_true(get_file(c4) == FILE_C);
	assert_true(get_file(d4) == FILE_D);
	assert_true(get_file(e4) == FILE_E);
	assert_true(get_file(f4) == FILE_F);
	assert_true(get_file(g4) == FILE_G);
	assert_true(get_file(h4) == FILE_H);

	assert_true(get_file(a5) == FILE_A);
	assert_true(get_file(b5) == FILE_B);
	assert_true(get_file(c5) == FILE_C);
	assert_true(get_file(d5) == FILE_D);
	assert_true(get_file(e5) == FILE_E);
	assert_true(get_file(f5) == FILE_F);
	assert_true(get_file(g5) == FILE_G);
	assert_true(get_file(h5) == FILE_H);

	assert_true(get_file(a6) == FILE_A);
	assert_true(get_file(b6) == FILE_B);
	assert_true(get_file(c6) == FILE_C);
	assert_true(get_file(d6) == FILE_D);
	assert_true(get_file(e6) == FILE_E);
	assert_true(get_file(f6) == FILE_F);
	assert_true(get_file(g6) == FILE_G);
	assert_true(get_file(h6) == FILE_H);

	assert_true(get_file(a7) == FILE_A);
	assert_true(get_file(b7) == FILE_B);
	assert_true(get_file(c7) == FILE_C);
	assert_true(get_file(d7) == FILE_D);
	assert_true(get_file(e7) == FILE_E);
	assert_true(get_file(f7) == FILE_F);
	assert_true(get_file(g7) == FILE_G);
	assert_true(get_file(h7) == FILE_H);

	assert_true(get_file(a8) == FILE_A);
	assert_true(get_file(b8) == FILE_B);
	assert_true(get_file(c8) == FILE_C);
	assert_true(get_file(d8) == FILE_D);
	assert_true(get_file(e8) == FILE_E);
	assert_true(get_file(f8) == FILE_F);
	assert_true(get_file(g8) == FILE_G);
	assert_true(get_file(h8) == FILE_H);
}



static void test_get_square_from_rank_file(void)
{
	assert_true(get_square(RANK_1, FILE_A) == a1);
	assert_true(get_square(RANK_1, FILE_B) == b1);
	assert_true(get_square(RANK_1, FILE_C) == c1);
	assert_true(get_square(RANK_1, FILE_D) == d1);
	assert_true(get_square(RANK_1, FILE_E) == e1);
	assert_true(get_square(RANK_1, FILE_F) == f1);
	assert_true(get_square(RANK_1, FILE_G) == g1);
	assert_true(get_square(RANK_1, FILE_H) == h1);

	assert_true(get_square(RANK_2, FILE_A) == a2);
	assert_true(get_square(RANK_2, FILE_B) == b2);
	assert_true(get_square(RANK_2, FILE_C) == c2);
	assert_true(get_square(RANK_2, FILE_D) == d2);
	assert_true(get_square(RANK_2, FILE_E) == e2);
	assert_true(get_square(RANK_2, FILE_F) == f2);
	assert_true(get_square(RANK_2, FILE_G) == g2);
	assert_true(get_square(RANK_2, FILE_H) == h2);

	assert_true(get_square(RANK_3, FILE_A) == a3);
	assert_true(get_square(RANK_3, FILE_B) == b3);
	assert_true(get_square(RANK_3, FILE_C) == c3);
	assert_true(get_square(RANK_3, FILE_D) == d3);
	assert_true(get_square(RANK_3, FILE_E) == e3);
	assert_true(get_square(RANK_3, FILE_F) == f3);
	assert_true(get_square(RANK_3, FILE_G) == g3);
	assert_true(get_square(RANK_3, FILE_H) == h3);

	assert_true(get_square(RANK_4, FILE_A) == a4);
	assert_true(get_square(RANK_4, FILE_B) == b4);
	assert_true(get_square(RANK_4, FILE_C) == c4);
	assert_true(get_square(RANK_4, FILE_D) == d4);
	assert_true(get_square(RANK_4, FILE_E) == e4);
	assert_true(get_square(RANK_4, FILE_F) == f4);
	assert_true(get_square(RANK_4, FILE_G) == g4);
	assert_true(get_square(RANK_4, FILE_H) == h4);

	assert_true(get_square(RANK_5, FILE_A) == a5);
	assert_true(get_square(RANK_5, FILE_B) == b5);
	assert_true(get_square(RANK_5, FILE_C) == c5);
	assert_true(get_square(RANK_5, FILE_D) == d5);
	assert_true(get_square(RANK_5, FILE_E) == e5);
	assert_true(get_square(RANK_5, FILE_F) == f5);
	assert_true(get_square(RANK_5, FILE_G) == g5);
	assert_true(get_square(RANK_5, FILE_H) == h5);

	assert_true(get_square(RANK_6, FILE_A) == a6);
	assert_true(get_square(RANK_6, FILE_B) == b6);
	assert_true(get_square(RANK_6, FILE_C) == c6);
	assert_true(get_square(RANK_6, FILE_D) == d6);
	assert_true(get_square(RANK_6, FILE_E) == e6);
	assert_true(get_square(RANK_6, FILE_F) == f6);
	assert_true(get_square(RANK_6, FILE_G) == g6);
	assert_true(get_square(RANK_6, FILE_H) == h6);

	assert_true(get_square(RANK_7, FILE_A) == a7);
	assert_true(get_square(RANK_7, FILE_B) == b7);
	assert_true(get_square(RANK_7, FILE_C) == c7);
	assert_true(get_square(RANK_7, FILE_D) == d7);
	assert_true(get_square(RANK_7, FILE_E) == e7);
	assert_true(get_square(RANK_7, FILE_F) == f7);
	assert_true(get_square(RANK_7, FILE_G) == g7);
	assert_true(get_square(RANK_7, FILE_H) == h7);

	assert_true(get_square(RANK_8, FILE_A) == a8);
	assert_true(get_square(RANK_8, FILE_B) == b8);
	assert_true(get_square(RANK_8, FILE_C) == c8);
	assert_true(get_square(RANK_8, FILE_D) == d8);
	assert_true(get_square(RANK_8, FILE_E) == e8);
	assert_true(get_square(RANK_8, FILE_F) == f8);
	assert_true(get_square(RANK_8, FILE_G) == g8);
	assert_true(get_square(RANK_8, FILE_H) == h8);

}



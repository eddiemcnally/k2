#include <stdbool.h>
#include <stdint.h>
#include "seatest.h"
#include "square.h"
#include "piece.h"
#include "move.h"
#include "test_move.h"

static void test_quiet_move_encode_decode(void);

void test_fixture_move(void)
{
    test_fixture_start(); // starts a fixture

    run_test(test_quiet_move_encode_decode);

    test_fixture_end(); // ends a fixture
}

static void test_quiet_move_encode_decode(void)
{
    for (enum square from_sq = a1; from_sq <= h8; from_sq++) {
        for (enum square to_sq = h8; to_sq >= a1; to_sq--) {

            //printf("from_sq %s\n", print_square(from_sq));
            //printf("to_sq %s\n", print_square(to_sq));

            struct move mv = move_encode_quiet(from_sq, to_sq);

            enum square decoded_from = move_decode_from_sq(&mv);
            enum square decoded_to = move_decode_to_sq(&mv);

            //printf("decoded from_sq %s\n", print_square(decoded_from));
            //printf("decoded to_sq %s\n", print_square(decoded_to));



            assert_true(from_sq == decoded_from);
            assert_true(to_sq == decoded_to);
        }
    }
}



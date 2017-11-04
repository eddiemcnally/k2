#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "square.h"
#include "piece.h"
#include "move.h"
#include "test_move.h"


void test_move_quiet_move_encode_decode(void **state)
{
    for (enum square from_sq = a1; from_sq <= h8; from_sq++) {
        for (enum square to_sq = a1; to_sq <= h8; to_sq++) {
            struct move mv = move_encode_quiet(from_sq, to_sq);

            enum square decoded_from = move_decode_from_sq(&mv);
            enum square decoded_to = move_decode_to_sq(&mv);

            assert_true(from_sq == decoded_from);
            assert_true(to_sq == decoded_to);
        }
    }
}

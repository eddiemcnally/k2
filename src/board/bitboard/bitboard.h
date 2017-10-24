#pragma once

#include <stdbool.h>
#include "square.h"


typedef uint64_t bitboard_t;

void bb_set_square(bitboard_t* bb, const enum square sq);
void bb_clear_square(bitboard_t* bb, const enum square sq);
bool bb_is_set(bitboard_t bb, const enum square sq);


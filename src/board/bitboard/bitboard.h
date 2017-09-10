#pragma once

#include "square.h"


typedef uint64_t bitboard_t;

void set_bit(bitboard_t* bb, const enum square sq);
void clear_bit(bitboard_t* bb, const enum square sq);

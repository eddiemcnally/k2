#pragma once

#include <stdint.h>

enum castle_perm {
    CAST_PERM_NONE  = 0,
    CAST_PERM_WK    = 0x01 << 0,
    CAST_PERM_WQ    = 0x01 << 1,
    CAST_PERM_BK    = 0x01 << 2,
    CAST_PERM_BQ    = 0x01 << 3,
};


#define INITIAL_FEN     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n"

void add_cast_perm(uint8_t* cp, const enum castle_perm perm);
void remove_cast_perm(uint8_t* cp, const enum castle_perm perm);
bool has_cast_perm(const uint8_t cp, const enum castle_perm perm);

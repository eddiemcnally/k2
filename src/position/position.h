#pragma once

#include <stdint.h>

enum castle_perm {
	CAST_PERM_NONE 	= 0,
	CAST_PERM_WK 	= 0x01,
	CAST_PERM_WQ 	= 0x02,
	CAST_PERM_BK 	= 0x04,
	CAST_PERM_BQ 	= 0x08,
};

typedef uint8_t	cast_perm_t;

#define INITIAL_FEN 	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n"

void add_cast_perm(cast_perm_t* cp, const enum castle_perm perm);
void remove_cast_perm(cast_perm_t* cp, const enum castle_perm perm);
bool has_cast_perm(const cast_perm_t cp, const enum castle_perm perm);

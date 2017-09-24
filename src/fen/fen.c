#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "piece.h"
#include "square.h"
#include "position.h"
#include "fen.h"

static struct piece_location {
	enum piece piece;
	enum square square;
}


static struct parsed_fen {
	struct piece_location pieces[NUM_SQUARES];

	enum colour side_to_move;

	bool is_castle_perm_set
	enum castle_perms castle_perm;

	bool is_en_pass_set;
	enum square en_pass_sq;

	uint16_t half_move_cnt;
	uint16_t full_move_cnt;

}
struct parsed_fen* parse_fen(const char* fen) {}

bool get_piece_on_sq(const struct parsed_fen* pf, const enum square sq, enum piece *pce) {}
bool get_castle_permissions(const struct parsed_fen* pf, enum castle_perms *cp) {}
bool get_en_pass_sq(const struct parsed_fen* pf, enum square *sq) {}
enum colour get_side_to_move(const struct parsed_fen* pf) {}
uint16_t get_half_move_cnt(const struct parsed_fen* pf) {}
uint16_t get_fill_move - cnt(const struct parsed_fen* pf) {}
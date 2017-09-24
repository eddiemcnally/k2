#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "piece.h"
#include "square.h"



struct parsed_fen;

struct parsed_fen* parse_fen(const char* fen);
bool get_piece_on_sq(const struct parsed_fen* pf, const enum square sq, enum piece *pce);
bool get_castle_permissions(const struct parsed_fen* pf, enum castle_perms *cp);
bool get_en_pass_sq(const struct parsed_fen* pf, enum square *sq);
enum colour get_side_to_move(const struct parsed_fen* pf);
uint16_t get_half_move_cnt(const struct parsed_fen* pf);
uint16_t get_fill_move - cnt(const struct parsed_fen* pf);
#pragma once

#include <stdbool.h>
#include "square.h"
#include "position.h"
#include "piece.h"

struct parsed_fen;

struct parsed_fen* parse_fen(const char* fen);
bool try_get_piece_on_sq(const struct parsed_fen* pf, const enum square sq, enum piece *pce);
cast_perm_t get_castle_permissions(const struct parsed_fen *pf);
bool try_get_en_pass_sq(const struct parsed_fen* pf, enum square *sq);
enum colour get_side_to_move(const struct parsed_fen* pf);
uint16_t get_half_move_cnt(const struct parsed_fen* pf);
uint16_t get_fill_move_cnt(const struct parsed_fen* pf);

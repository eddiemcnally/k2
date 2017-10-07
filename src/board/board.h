#pragma once

#include <stdbool.h>
#include "piece.h"
#include "square.h"

struct board;


bool is_sq_occupied(const struct board* brd);
void add_piece(struct board* brd, const enum piece pce, const enum square sq);
void remove_piece(struct board* brd, const enum piece pce, const enum square sq);
void move_piece(struct board* brd, const enum piece pce, const enum square from_sq, const enum square to_sq);

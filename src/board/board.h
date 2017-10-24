#pragma once

#include <stdbool.h>
#include "piece.h"
#include "square.h"
#include "board.h"


struct board;

struct board* get_empty_board(void);
bool is_sq_occupied(const struct board* brd, const enum square sq);
void add_piece(struct board* brd, const enum piece pce, const enum square sq);
void remove_piece(struct board* brd, const enum piece pce, const enum square sq);
void move_piece(struct board* brd, const enum piece pce, const enum square from_sq, const enum square to_sq);

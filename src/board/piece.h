#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "piece.h"


// Definition of pieces.
// see https://chessprogramming.wikispaces.com/Pieces

enum colour {
    // colours
    WHITE = 0x01,
    BLACK = 0x02,
    COLOUR_MASK = 0x03
};

/*
 *  0000 0001   WHITE
 *  0000 0010   BLACK
 *  0000 0100   PAWN
 *  0000 1000   KNIGHT
 *  0001 0000   BISHOP
 *  0010 0000   ROOK
 *  0100 0000   QUEEN
 *  1000 000    KING
 */
enum piece {
    // piece type
    PAWN = 0x04,
    KNIGHT = 0x08,
    BISHOP = 0x10,
    ROOK = 0x20,
    QUEEN = 0x40,
    KING = 0x80,
    // white pieces
    WPAWN = (PAWN | WHITE),
    WBISHOP = (BISHOP | WHITE),
    WKNIGHT = (KNIGHT | WHITE),
    WROOK = (ROOK | WHITE),
    WQUEEN = (QUEEN | WHITE),
    WKING = (KING | WHITE),
    // black pieces
    BPAWN = (PAWN | BLACK),
    BBISHOP = (BISHOP | BLACK),
    BKNIGHT = (KNIGHT | BLACK),
    BROOK = (ROOK | BLACK),
    BQUEEN = (QUEEN | BLACK),
    BKING = (KING | BLACK),
};



#define NUM_PIECES  12
#define NUM_COLOURS  2

bool pce_is_white(const enum piece pce);
bool pce_is_black(const enum piece pce);
enum colour swap_side(const enum colour side);
enum colour pce_get_colour(const enum piece pce);
uint32_t pce_get_value(const enum piece pce);
enum piece pce_get_piece_type(const enum piece piece);
char pce_get_label(const enum piece pce);
enum piece pce_get_from_label(const char c);
void validate_piece(const enum piece pce);
void validate_colour(const enum colour col);
void validate_label(const char c);




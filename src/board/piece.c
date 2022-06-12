/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without 
 *  restriction, including without limitation the rights to use, 
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following 
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*! @addtogroup Piece
 *
 * @ingroup Piece
 * @{
 * @details This contains code associated with a piece, colour, value, role, etc.
 *
 */

#include "piece.h"
#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Piece values
 * values taken from here: 
 * https://www.chessprogramming.org/Simplified_Evaluation_Function 
 */
// clang-format off
enum piece_values {
    PCE_VAL_PAWN    = 100,
    PCE_VAL_BISHOP  = 330,
    PCE_VAL_KNIGHT  = 320,
    PCE_VAL_ROOK    = 500,
    PCE_VAL_QUEEN   = 900,
    PCE_VAL_KING    = 20000
};

struct piece {
    uint8_t             offset;
    enum piece_role     role;
    enum colour         colour;
    uint16_t            value;
    char                label;
};


// array offsets
enum offsets{
    WP,
    WB,
    WN,
    WR,
    WQ,
    WK,
    BP,
    BB,
    BN,
    BR,
    BQ,
    BK,    
};


static const struct piece WHITE_PAWN    = {.role = PAWN,    .colour = WHITE, .value = PCE_VAL_PAWN,    .offset = WP, .label = 'P'};
static const struct piece WHITE_BISHOP  = {.role = BISHOP,  .colour = WHITE, .value = PCE_VAL_BISHOP,  .offset = WB, .label = 'B'};
static const struct piece WHITE_KNIGHT  = {.role = KNIGHT,  .colour = WHITE, .value = PCE_VAL_KNIGHT,  .offset = WN, .label = 'N'};
static const struct piece WHITE_ROOK    = {.role = ROOK,    .colour = WHITE, .value = PCE_VAL_ROOK,    .offset = WR, .label = 'R'};
static const struct piece WHITE_QUEEN   = {.role = QUEEN,   .colour = WHITE, .value = PCE_VAL_QUEEN,   .offset = WQ, .label = 'Q'};
static const struct piece WHITE_KING    = {.role = KING,    .colour = WHITE, .value = PCE_VAL_KING,    .offset = WK, .label = 'K'};

static const struct piece BLACK_PAWN    = {.role = PAWN,    .colour = BLACK, .value = PCE_VAL_PAWN,    .offset = BP, .label = 'p'};
static const struct piece BLACK_BISHOP  = {.role = BISHOP,  .colour = BLACK, .value = PCE_VAL_BISHOP,  .offset = BB, .label = 'b'};
static const struct piece BLACK_KNIGHT  = {.role = KNIGHT,  .colour = BLACK, .value = PCE_VAL_KNIGHT,  .offset = BN, .label = 'n'};
static const struct piece BLACK_ROOK    = {.role = ROOK,    .colour = BLACK, .value = PCE_VAL_ROOK,    .offset = BR, .label = 'r'};
static const struct piece BLACK_QUEEN   = {.role = QUEEN,   .colour = BLACK, .value = PCE_VAL_QUEEN,   .offset = BQ, .label = 'q'};
static const struct piece BLACK_KING    = {.role = KING,    .colour = BLACK, .value = PCE_VAL_KING,    .offset = BK, .label = 'k'};


//
// ==================================================================
//
// public functions
//
// ==================================================================

__attribute__((always_inline)) const struct piece *pce_get_white_pawn(void)     { return &WHITE_PAWN;   }
__attribute__((always_inline)) const struct piece *pce_get_black_pawn(void)     { return &BLACK_PAWN;   }
__attribute__((always_inline)) const struct piece *pce_get_white_bishop(void)   { return &WHITE_BISHOP; }
__attribute__((always_inline)) const struct piece *pce_get_black_bishop(void)   { return &BLACK_BISHOP; }
__attribute__((always_inline)) const struct piece *pce_get_white_knight(void)   { return &WHITE_KNIGHT; }
__attribute__((always_inline)) const struct piece *pce_get_black_knight(void)   { return &BLACK_KNIGHT; }
__attribute__((always_inline)) const struct piece *pce_get_white_rook(void)     { return &WHITE_ROOK;   }
__attribute__((always_inline)) const struct piece *pce_get_black_rook(void)     { return &BLACK_ROOK;   }
__attribute__((always_inline)) const struct piece *pce_get_white_queen(void)    { return &WHITE_QUEEN;  }
__attribute__((always_inline)) const struct piece *pce_get_black_queen(void)    { return &BLACK_QUEEN;  }
__attribute__((always_inline)) const struct piece *pce_get_white_king(void)     { return &WHITE_KING;   }
__attribute__((always_inline)) const struct piece *pce_get_black_king(void)     { return &BLACK_KING;   }

// clang-format on

const struct piece *pce_get(const enum piece_role role, const enum colour colour) {

    if (colour == WHITE) {
        switch (role) {
        case PAWN:
            return &WHITE_PAWN;
        case BISHOP:
            return &WHITE_BISHOP;
        case KNIGHT:
            return &WHITE_KNIGHT;
        case ROOK:
            return &WHITE_ROOK;
        case QUEEN:
            return &WHITE_QUEEN;
        case KING:
            return &WHITE_KING;
        default:
            REQUIRE(false, "Invalid piece_role");
        }
    } else if (colour == BLACK) {
        switch (role) {
        case PAWN:
            return &BLACK_PAWN;
        case BISHOP:
            return &BLACK_BISHOP;
        case KNIGHT:
            return &BLACK_KNIGHT;
        case ROOK:
            return &BLACK_ROOK;
        case QUEEN:
            return &BLACK_QUEEN;
        case KING:
            return &BLACK_KING;
        default:
            REQUIRE(false, "Invalid piece_role");
        }
    } else {
        REQUIRE(false, "invalid colour");
    }
}

/**
 * @brief       Takes a given colour/side, and returns the other colour/side
 *
 * @param col   The given colour
 * @return      The opposite colour
 */
enum colour pce_swap_side(const enum colour col) {
    assert(validate_colour(col));

    return (enum colour)(~col & 0x01);
}

uint32_t pce_get_value(const struct piece *const piece) {
    return piece->value;
}

__attribute__((always_inline)) enum colour pce_get_colour(const struct piece *const piece) {
    assert(validate_piece(piece));
    return piece->colour;
}
__attribute__((always_inline)) enum piece_role pce_get_role(const struct piece *const piece) {
    assert(validate_piece(piece));
    return piece->role;
}

__attribute__((always_inline)) uint8_t pce_get_array_offset(const struct piece *const piece) {
    assert(validate_piece(piece));
    return piece->offset;
}

char pce_get_label(const struct piece *const piece) {
    assert(validate_piece(piece));
    return piece->label;
}

const struct piece *pce_get_from_label(const char c) {

    switch (c) {
    case 'P':
        return &WHITE_PAWN;
    case 'B':
        return &WHITE_BISHOP;
    case 'N':
        return &WHITE_KNIGHT;
    case 'R':
        return &WHITE_ROOK;
    case 'Q':
        return &WHITE_QUEEN;
    case 'K':
        return &WHITE_KING;
    case 'p':
        return &BLACK_PAWN;
    case 'b':
        return &BLACK_BISHOP;
    case 'n':
        return &BLACK_KNIGHT;
    case 'r':
        return &BLACK_ROOK;
    case 'q':
        return &BLACK_QUEEN;
    case 'k':
        return &BLACK_KING;
    default:
        REQUIRE(false, "Invalid piece  label");
    }
}

/**
 * @brief       Validates a colour is within expected range of values
 *
 * @param col The colour
 */
bool validate_colour(const enum colour col) {
    switch (col) {
    case WHITE:
    case BLACK:
        return true;
    default:
        return false;
    }
}

bool validate_piece_role(const enum piece_role role) {
    switch (role) {
    case PAWN:
    case BISHOP:
    case KNIGHT:
    case ROOK:
    case QUEEN:
    case KING:
        return true;
    default:
        return false;
    }
}

bool validate_label(const char label) {
    switch (label) {
    case 'P':
    case 'B':
    case 'N':
    case 'R':
    case 'Q':
    case 'K':
    case 'p':
    case 'b':
    case 'n':
    case 'r':
    case 'q':
    case 'k':
        return true;
    default:
        return false;
    }
}

bool validate_piece(const struct piece *const piece) {

    if (validate_label(piece->label) == false) {
        return false;
    }
    if (validate_piece_role(piece->role) == false) {
        return false;
    }
    if (validate_colour(piece->colour) == false) {
        return false;
    }
    return true;
}

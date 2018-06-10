/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */


#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "piece.h"


enum colour {
        // colours
        WHITE   = 0x01,
        BLACK   = 0x02,
        NUM_COLOURS = 2
};


/**
 * Definition of pieces; see https://chessprogramming.wikispaces.com/Pieces
 *  0000 0001   WHITE
 *  0000 0010   BLACK
 *  0000 0100   PAWN
 *  0000 1000   KNIGHT
 *  0001 0000   BISHOP
 *  0010 0000   ROOK
 *  0100 0000   QUEEN
 *  1000 0000   KING
 */
enum piece_class {
        // piece type
        PAWN    = 0x04,
        KNIGHT  = 0x08,
        BISHOP  = 0x10,
        ROOK    = 0x20,
        QUEEN   = 0x40,
        KING    = 0x80,
};

enum piece {
        // white pieces
        WPAWN   = ( PAWN | WHITE ),
        WBISHOP = ( BISHOP | WHITE ),
        WKNIGHT = ( KNIGHT | WHITE ),
        WROOK   = ( ROOK | WHITE ),
        WQUEEN  = ( QUEEN | WHITE ),
        WKING   = ( KING | WHITE ),
        // black pieces
        BPAWN   = ( PAWN | BLACK ),
        BBISHOP = ( BISHOP | BLACK ),
        BKNIGHT = ( KNIGHT | BLACK ),
        BROOK   = ( ROOK | BLACK ),
        BQUEEN  = ( QUEEN | BLACK ),
        BKING   = ( KING | BLACK ),
};




#define     NUM_PIECES          12
#define     NUM_PIECE_CLASSES   6


bool pce_is_white ( const enum piece pce );
bool pce_is_black ( const enum piece pce );
enum colour swap_side ( const enum colour side );
enum colour pce_get_colour ( const enum piece pce );
uint32_t pce_get_value ( const enum piece pce );
enum piece_class pce_get_piece_class ( const enum piece piece );
char pce_get_label ( const enum piece pce );
enum piece pce_get_from_label ( const char c );
uint8_t pce_get_array_idx ( const enum piece pce );
uint8_t pce_col_get_array_idx ( const enum colour col );
enum piece pce_get_no_piece ( void );
enum piece pce_get_piece ( const enum piece_class pc, const enum colour col );
bool validate_piece ( const enum piece pce );
bool validate_colour ( const enum colour col );
bool validate_label ( const char c );
bool validate_piece_class ( const enum piece_class pc );

// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

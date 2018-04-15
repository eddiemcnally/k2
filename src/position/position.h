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
#include "move.h"
#include "square.h"
#include "castle_perms.h"

struct position;


#define INITIAL_FEN     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n"

cast_perm_t pos_get_cast_perm ( const struct position *pos );
void pos_set_cast_perm ( struct position *pos, const cast_perm_t perms );

struct position * pos_create ( void );
void pos_destroy ( struct position *pos );
void pos_initialise ( const char * fen, struct position *pos );

struct board * pos_get_board ( const struct position *pos );
bool pos_try_get_en_pass_sq ( const struct position *pos, enum square *en_pass_sq );
enum colour pos_get_side_to_move ( const struct position *pos );

bool validate_position ( const struct position *pos );

// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

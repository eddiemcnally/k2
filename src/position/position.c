// Copyright (c) 2017 Eddie McNally
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "position.h"
#include "board.h"




#define STRUCT_INIT_KEY     ((uint64_t)0xdeadbeef)

struct position
{
    uint64_t struct_init_key;

    // current board representation
    struct board *brd;

    // the next side to move
    enum colour side_to_move;

    // a hash of the current board
    uint64_t board_hash;

    // the square where en passent is active
    enum square en_passant;

    uint8_t fifty_move_counter;

    // keeping track of ply
    uint8_t ply;
    uint8_t history_ply;

    // castling permissions
    uint8_t castle_perm;
};




struct position * pos_create(const char * fen)
{
    return NULL;
}

void pos_destroy(struct position *pos)
{

}

void add_cast_perm(uint8_t* cp, const enum castle_perm perm)
{
    if (perm == CAST_PERM_NONE)
    {
        *cp = CAST_PERM_NONE;
    }
    else
    {
        *cp = *cp | (uint8_t) perm;
    }
}

void remove_cast_perm(uint8_t* cp, const enum castle_perm perm)
{
    *cp = *cp & (~perm);
}

bool has_cast_perm(const uint8_t cp, const enum castle_perm perm)
{
    if (perm == CAST_PERM_NONE)
    {
        return (cp == 0);
    }
    return (cp & perm) != 0;
}

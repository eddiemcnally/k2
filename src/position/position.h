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

#pragma once

#include <stdint.h>

struct position;

enum castle_perm
{
	CAST_PERM_NONE  = 0,
	CAST_PERM_WK    = 0x01 << 0,
	CAST_PERM_WQ    = 0x01 << 1,
	CAST_PERM_BK    = 0x01 << 2,
	CAST_PERM_BQ    = 0x01 << 3,
};


#define INITIAL_FEN     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n"

void add_cast_perm(uint8_t* cp, const enum castle_perm perm);
void remove_cast_perm(uint8_t* cp, const enum castle_perm perm);
bool has_cast_perm(const uint8_t cp, const enum castle_perm perm);

struct position * pos_create(const char * fen);
void pos_destroy(struct position *pos);


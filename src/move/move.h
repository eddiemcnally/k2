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

#include "square.h"
#include "piece.h"


typedef uint16_t move_t;


move_t move_encode_quiet(const enum square from_sq, const enum square to_sq);
move_t move_encode_promoted(const enum square from_sq, const enum square to_sq, const enum piece promoted_piece, const bool is_capture);
move_t move_encode_capture(const enum square from_sq, const enum square to_sq);
move_t move_encode_enpassant(const enum square from_sq, const enum square to_sq);
enum square move_decode_from_sq(const move_t mv);
enum square move_decode_to_sq(const move_t mv);

bool move_is_quiet(move_t mv);
bool move_is_capture(move_t mv);
bool move_is_promotion(move_t mv);
bool move_is_en_passant(move_t mv);


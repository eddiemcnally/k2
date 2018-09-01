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


uint16_t move_encode_quiet ( const enum square from_sq, const enum square to_sq );
uint16_t move_encode_promoted ( const enum square from_sq, const enum square to_sq, const enum piece promoted_piece, const bool is_capture );
uint16_t move_encode_capture ( const enum square from_sq, const enum square to_sq );
uint16_t move_encode_enpassant ( const enum square from_sq, const enum square to_sq );
uint16_t move_encode_pawn_double_first ( const enum square from_sq, const enum square to_sq );
uint16_t move_encode_castle_kingside_white ( void );
uint16_t move_encode_castle_kingside_black ( void );
uint16_t move_encode_castle_queenside_white ( void );
uint16_t move_encode_castle_queenside_black ( void );

enum square move_decode_from_sq ( const uint16_t mv );
enum square move_decode_to_sq ( const uint16_t mv );
enum piece move_decode_promotion_piece ( const uint16_t mv , const enum colour side );
bool move_is_quiet ( const uint16_t mv );
bool move_is_capture ( const uint16_t mv );
bool move_is_promotion ( const uint16_t mv );
bool move_is_en_passant ( const uint16_t mv );
bool move_is_castle ( const uint16_t mv );
bool move_is_double_pawn ( const uint16_t mv );
bool move_is_king_castle ( const uint16_t mv );
bool move_is_queen_castle ( const uint16_t mv );
enum piece move_get_promote_piece ( const uint16_t mv, const enum colour side_being_moved );




char *move_print ( uint16_t mv );

bool validate_move ( const uint16_t mv );

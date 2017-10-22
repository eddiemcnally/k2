#pragma once

#include "square.h"

/**
 * bitmap for the struct
 * See http://chessprogramming.wikispaces.com/Encoding+Moves
 *
 * ---- ---- --11 1111      To Square
 * ---- 1111 11-- ----      From Square
 * 0000 ---- ---- ----      Quiet move
 * 0001 ---- ---- ----      Double Pawn push
 * 0010 ---- ---- ----      King Castle
 * 0011 ---- ---- ----      Queen Castle
 * 0100 ---- ---- ----      Capture
 * 0101 ---- ---- ----      En Passant Capture
 * 1000 ---- ---- ----      Promotion Knight
 * 1001 ---- ---- ----      Promotion Bishop
 * 1010 ---- ---- ----      Promotion Rook
 * 1011 ---- ---- ----      Promotion Queen
 * 1100 ---- ---- ----      Promotion Knight Capture
 * 1101 ---- ---- ----      Promotion Bishop Capture
 * 1110 ---- ---- ----      Promotion Rook Capture
 * 1111 ---- ---- ----      Promotion Queen Capture
 */
struct move {
    uint16_t mv;
};

struct move move_encode_quiet(const enum square from_sq, const enum square to_sq);
struct move move_encode_promoted(const enum square from_sq, const enum square to_sq, const enum piece promoted_piece, const bool is_capture);
struct move move_encode_capture(const enum square from_sq, const enum square to_sq);
struct move move_encode_enpassant(const enum square from_sq, const enum square to_sq);
enum square move_decode_from_sq(const struct move *mv);
enum square move_decode_to_sq(const struct move *mv);

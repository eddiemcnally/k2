#pragma once

struct move encode_move_quiet(const enum square from_sq, const enum square to_sq);
struct move encode_move_promoted(const enum square from_sq, const enum square to_sq, const enum piece promoted_piece, const bool is_capture);
struct move encode_move_capture(const enum square from_sq, const enum square to_sq);
struct move encode_move_enpassant(const enum square from_sq, const enum square to_sq);
enum square decode_move_from_sq(const struct move mv);
enum square decode_move_to_sq(const struct move mv);

struct move;


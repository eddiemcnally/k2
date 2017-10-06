#pragma once

bitboard_t get_knight_occ_mask(const enum square sq);
bitboard_t get_bishop_occ_mask(const enum square sq);
bitboard_t get_king_occ_mask(const enum square sq);
bitboard_t get_queen_occ_mask(const enum square sq);
bitboard_t get_rook_occ_mask(const enum square sq);


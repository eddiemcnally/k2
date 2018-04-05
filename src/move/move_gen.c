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
#include <stdio.h>
#include <assert.h>
#include "move_gen.h"
#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "castle_perms.h"
#include "move_list.h"
#include "occupancy_mask.h"
#include "piece.h"
#include "position.h"




/* indexed using enum square
 * Represents the horizontal squares that a rook can move to, when
 * on a specific square
 */
static const uint64_t horizontal_move_mask[] = {
        0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff,
        0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff,
        0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00,
        0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00,
        0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000,
        0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000,
        0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000,
        0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000,
        0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000,
        0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000,
        0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000,
        0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000,
        0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000,
        0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000,
        0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000,
        0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000
};

/* indexed using enum square
 * Represents the vertical squares that a rook can move to, when
 * on a specific square
 */
static const uint64_t vertical_move_mask[] = {
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080
};


/* indexed using enum square
 * Represents the bottom-left to top-right diagonals that a bishop can move to, when
 * on a specific square
 */
static const uint64_t positive_diagonal_masks[] = {
        0x8040201008040200, 0x0080402010080400, 0x0000804020100800, 0x0000008040201000,
        0x0000000080402000, 0x0000000000804000, 0x0000000000008000, 0x0000000000000000,
        0x4020100804020000, 0x8040201008040001, 0x0080402010080002, 0x0000804020100004,
        0x0000008040200008, 0x0000000080400010, 0x0000000000800020, 0x0000000000000040,
        0x2010080402000000, 0x4020100804000100, 0x8040201008000201, 0x0080402010000402,
        0x0000804020000804, 0x0000008040001008, 0x0000000080002010, 0x0000000000004020,
        0x1008040200000000, 0x2010080400010000, 0x4020100800020100, 0x8040201000040201,
        0x0080402000080402, 0x0000804000100804, 0x0000008000201008, 0x0000000000402010,
        0x0804020000000000, 0x1008040001000000, 0x2010080002010000, 0x4020100004020100,
        0x8040200008040201, 0x0080400010080402, 0x0000800020100804, 0x0000000040201008,
        0x0402000000000000, 0x0804000100000000, 0x1008000201000000, 0x2010000402010000,
        0x4020000804020100, 0x8040001008040201, 0x0080002010080402, 0x0000004020100804,
        0x0200000000000000, 0x0400010000000000, 0x0800020100000000, 0x1000040201000000,
        0x2000080402010000, 0x4000100804020100, 0x8000201008040201, 0x0000402010080402,
        0x0000000000000000, 0x0001000000000000, 0x0002010000000000, 0x0004020100000000,
        0x0008040201000000, 0x0010080402010000, 0x0020100804020100, 0x0040201008040201
};

/* indexed using enum square
 * Represents the top-left to bottom-right diagonals that a bishop can move to, when
 * on a specific square
 */
static const uint64_t negative_diagonal_masks[] = {
        0x0000000000000000, 0x0000000000000100, 0x0000000000010200, 0x0000000001020400,
        0x0000000102040800, 0x0000010204081000, 0x0001020408102000, 0x0102040810204000,
        0x0000000000000002, 0x0000000000010004, 0x0000000001020008, 0x0000000102040010,
        0x0000010204080020, 0x0001020408100040, 0x0102040810200080, 0x0204081020400000,
        0x0000000000000204, 0x0000000001000408, 0x0000000102000810, 0x0000010204001020,
        0x0001020408002040, 0x0102040810004080, 0x0204081020008000, 0x0408102040000000,
        0x0000000000020408, 0x0000000100040810, 0x0000010200081020, 0x0001020400102040,
        0x0102040800204080, 0x0204081000408000, 0x0408102000800000, 0x0810204000000000,
        0x0000000002040810, 0x0000010004081020, 0x0001020008102040, 0x0102040010204080,
        0x0204080020408000, 0x0408100040800000, 0x0810200080000000, 0x1020400000000000,
        0x0000000204081020, 0x0001000408102040, 0x0102000810204080, 0x0204001020408000,
        0x0408002040800000, 0x0810004080000000, 0x1020008000000000, 0x2040000000000000,
        0x0000020408102040, 0x0100040810204080, 0x0200081020408000, 0x0400102040800000,
        0x0800204080000000, 0x1000408000000000, 0x2000800000000000, 0x4000000000000000,
        0x0002040810204080, 0x0004081020408000, 0x0008102040800000, 0x0010204080000000,
        0x0020408000000000, 0x0040800000000000, 0x0080000000000000, 0x0000000000000000
};

#define GET_DIAGONAL_MASK(sq)		(positive_diagonal_masks[sq])
#define GET_ANTI_DIAGONAL_MASK(sq)	(negative_diagonal_masks[sq])
#define GET_VERTICAL_MASK(sq) 		(vertical_move_mask[sq])
#define GET_HORIZONTAL_MASK(sq)		(horizontal_move_mask[sq])


// Bitboards representing commonly used ranks and files
static const bitboard_t RANK_2_BB      = 0x000000000000FF00;
static const bitboard_t RANK_7_BB      = 0x00FF000000000000;


// bitboards for squares between castle squares
static const bitboard_t CASTLE_MASK_WK = 0x0000000000000060;
static const bitboard_t CASTLE_MASK_WQ = 0x000000000000000E;
static const bitboard_t CASTLE_MASK_BK = 0x6000000000000000;
static const bitboard_t CASTLE_MASK_BQ = 0x0E00000000000000;

// target promotion pieces
#define         NUM_PROMOTION_PIECES    4
static enum piece WHITE_PROMOTION_PIECES [NUM_PROMOTION_PIECES] = {BKNIGHT, BBISHOP, BROOK, BQUEEN};
static enum piece BLACK_PROMOTION_PIECES [NUM_PROMOTION_PIECES] = {WKNIGHT, WBISHOP, WROOK, WQUEEN};


static void mv_gen_encode_multiple_quiet ( bitboard_t bb, const enum square from_sq, struct move_list *mvl );
static void mv_gen_encode_multiple_capture ( bitboard_t bb, const enum square from_sq, struct move_list *mvl );
static void mv_gen_king_knight_moves ( const struct board *brd, const enum piece pce_to_move, const enum colour side_to_move,  struct move_list *mvl );
static void mv_gen_black_castle_moves ( const struct position *pos, struct move_list *mvl );
static void mv_gen_white_castle_moves ( const struct position *pos, struct move_list *mvl );
static void add_kingside_move_if_no_blockers ( const bitboard_t brd_bb, const bitboard_t blocking_pce_mask, const enum colour side_to_move, struct move_list *mvl );
static void add_queenside_move_if_no_blockers ( const bitboard_t brd_bb, const bitboard_t blocking_pce_mask, const enum colour side_to_move, struct move_list *mvl );
static void gen_promotions ( const enum square from_sq, const enum square to_sq, struct move_list *mvl, const bool is_capture, const enum piece *promotion_pieces );
static void gen_white_pawn_moves_excl_first_double_move ( const struct position *pos, const struct board *brd, struct move_list *mvl );
static void gen_black_pawn_moves_excl_first_double_move ( const struct position *pos, const struct board *brd, struct move_list *mvl );
static void gen_black_pawn_double_first_move ( const struct board *brd, struct move_list *mvl );
static void gen_white_pawn_double_first_move ( const struct board *brd, struct move_list *mvl );
static void try_encode_double_pawn_move ( const struct board *brd, const enum square from_sq, const enum square plus_1, const enum square plus_2, struct move_list *mvl );



/**
 * @brief       Generates all valid moves for the given position
 * @details     Appends all new moves to the given move list
 * @param pos   The position
 * @param mvl   The move list
 */
void mv_gen_all_moves ( const struct position *pos, struct move_list *mvl )
{
        assert ( validate_position ( pos ) );
        assert ( validate_move_list ( mvl ) );

        struct board *brd = pos_get_board ( pos );
        enum colour side_to_move = pos_get_side_to_move ( pos );

        mv_gen_knight_moves ( brd, side_to_move, mvl );

        if ( side_to_move == WHITE ) {
                mv_gen_white_pawn_moves ( pos, brd, mvl );
        }

}

/**
 * @brief       Generates Knight moves
 * @details     Generates all valid Knight moves from the given
 *              position. New moves are appended to the given move list
 * @param brd   The board
 * @param side_to_move The side to move
 * @param mvl   the move list to append new moves to
 */
void mv_gen_knight_moves ( const struct board *brd, const enum colour side_to_move,  struct move_list *mvl )
{
        assert ( validate_board ( brd ) );
        assert ( validate_colour ( side_to_move ) );
        assert ( validate_move_list ( mvl ) );

        enum piece knight = ( side_to_move == WHITE ) ? WKNIGHT : BKNIGHT;

        mv_gen_king_knight_moves ( brd, knight, side_to_move, mvl );
}

/**
 * @brief       Generates White Pawn moves
 * @param brd   The board
 * @param mvl   the move list to append new moves to
 */
void mv_gen_white_pawn_moves ( const struct position *pos, const struct board *brd, struct move_list *mvl )
{
        assert ( validate_position ( pos ) );
        assert ( validate_board ( brd ) );
        assert ( validate_move_list ( mvl ) );

        gen_white_pawn_moves_excl_first_double_move ( pos, brd, mvl );
        gen_white_pawn_double_first_move ( brd, mvl );
}



/**
 * @brief       Generates Black Pawn moves
 * @param brd   The board
 * @param mvl   the move list to append new moves to
 */
void mv_gen_black_pawn_moves ( const struct position *pos, const struct board *brd, struct move_list *mvl )
{
        assert ( validate_position ( pos ) );
        assert ( validate_board ( brd ) );
        assert ( validate_move_list ( mvl ) );

        gen_black_pawn_moves_excl_first_double_move ( pos, brd, mvl );
        gen_black_pawn_double_first_move ( brd, mvl );
}



static void gen_white_pawn_double_first_move ( const struct board *brd, struct move_list *mvl )
{
        const bitboard_t all_pawns_bb = brd_get_piece_bb ( brd, WPAWN );

        // mask out all pawns other than on rank 2
        bitboard_t pawns_on_rank_2 = all_pawns_bb & RANK_2_BB;

        while ( pawns_on_rank_2 != 0 ) {
                enum square from_sq = bb_pop_1st_bit ( &pawns_on_rank_2 );
                enum square from_plus_1 = sq_get_square_plus_1_rank ( from_sq );
                enum square from_plus_2 = sq_get_square_plus_1_rank ( from_plus_1 );
                try_encode_double_pawn_move ( brd, from_sq, from_plus_1, from_plus_2, mvl );
        }
}


static void gen_black_pawn_double_first_move ( const struct board *brd, struct move_list *mvl )
{
        const bitboard_t all_pawns_bb = brd_get_piece_bb ( brd, BPAWN );

        // mask out all pawns other than on rank 7
        bitboard_t pawns_on_rank_7 = all_pawns_bb & RANK_7_BB;

        while ( pawns_on_rank_7 != 0 ) {
                enum square from_sq = bb_pop_1st_bit ( &pawns_on_rank_7 );
                enum square from_minus_1 = sq_get_square_minus_1_rank ( from_sq );
                enum square from_minus_2 = sq_get_square_minus_1_rank ( from_minus_1 );
                try_encode_double_pawn_move ( brd, from_sq, from_minus_1, from_minus_2, mvl );
        }
}


static void try_encode_double_pawn_move ( const struct board *brd, const enum square from_sq, const enum square plus_1,
                const enum square plus_2, struct move_list *mvl )
{
        if ( brd_is_sq_occupied ( brd, plus_1 ) == false && brd_is_sq_occupied ( brd, plus_2 ) == false ) {
                move_t quiet_move = move_encode_pawn_double_first ( from_sq, plus_2 );
                mvl_add ( mvl, quiet_move );
        }
}



static void gen_white_pawn_moves_excl_first_double_move ( const struct position *pos, const struct board *brd, struct move_list *mvl )
{
        enum square to_sq;
        enum square from_sq;
        enum square en_pass_sq;

        const bitboard_t all_pawns_bb = brd_get_piece_bb ( brd, WPAWN );
        const bitboard_t black_bb = brd_get_colour_bb ( brd, BLACK );
        const bitboard_t all_occupied_squares_bb = brd_get_board_bb ( brd );
        const bool is_en_pass_active = pos_try_get_en_pass_sq ( pos, &en_pass_sq );

        bitboard_t pawns_excl_rank7_bb = all_pawns_bb & ( ~RANK_7_BB );

        while ( pawns_excl_rank7_bb != 0 ) {
                // quiet moves
                from_sq = bb_pop_1st_bit ( &pawns_excl_rank7_bb );
                to_sq = sq_get_square_plus_1_rank ( from_sq );
                if ( bb_is_set ( all_occupied_squares_bb, to_sq ) == false ) {
                        move_t quiet_move = move_encode_quiet ( from_sq, to_sq );
                        mvl_add ( mvl, quiet_move );
                }

                // capture moves
                const bitboard_t occ_mask = occ_mask_get_white_pawn_capture_non_first_double_move ( from_sq );
                bitboard_t capt_bb = black_bb & occ_mask;
                while ( capt_bb != 0 ) {
                        to_sq = bb_pop_1st_bit ( &capt_bb );
                        move_t capt_move = move_encode_capture ( from_sq, to_sq );
                        mvl_add ( mvl, capt_move );
                }

                if ( is_en_pass_active ) {
                        bitboard_t en_pass_bb;
                        bb_set_square ( &en_pass_bb, en_pass_sq );
                        if ( ( en_pass_bb & occ_mask ) != 0 ) {
                                // en passant move available from this square
                                move_t en_pass_move = move_encode_enpassant ( from_sq, en_pass_sq );
                                mvl_add ( mvl, en_pass_move );
                        }
                }
        }

        // now check for promotion
        bitboard_t pawns_on_rank7_bb = all_pawns_bb & RANK_7_BB;
        while ( pawns_on_rank7_bb != 0 ) {
                // quiet promotion
                from_sq = bb_pop_1st_bit ( &pawns_on_rank7_bb );
                to_sq = sq_get_square_plus_1_rank ( from_sq );
                if ( bb_is_set ( all_occupied_squares_bb, to_sq ) == false ) {
                        gen_promotions ( from_sq, to_sq, mvl, false, WHITE_PROMOTION_PIECES );
                }

                // capture promotion
                const bitboard_t occ_mask = occ_mask_get_white_pawn_capture_non_first_double_move ( from_sq );
                bitboard_t capt_bb = black_bb & occ_mask;
                while ( capt_bb != 0 ) {
                        to_sq = bb_pop_1st_bit ( &capt_bb );
                        gen_promotions ( from_sq, to_sq, mvl, true, WHITE_PROMOTION_PIECES );
                }
        }
}



static void gen_black_pawn_moves_excl_first_double_move ( const struct position *pos, const struct board *brd, struct move_list *mvl )
{
        enum square to_sq;
        enum square from_sq;
        enum square en_pass_sq;

        const bitboard_t all_pawns_bb = brd_get_piece_bb ( brd, BPAWN );
        const bitboard_t white_bb = brd_get_colour_bb ( brd, WHITE );
        const bitboard_t all_occupied_squares_bb = brd_get_board_bb ( brd );
        const bool is_en_pass_active = pos_try_get_en_pass_sq ( pos, &en_pass_sq );

        bitboard_t pawns_excl_rank2_bb = all_pawns_bb & ( ~RANK_2_BB );

        while ( pawns_excl_rank2_bb != 0 ) {
                // quiet moves
                from_sq = bb_pop_1st_bit ( &pawns_excl_rank2_bb );
                to_sq = sq_get_square_minus_1_rank ( from_sq );
                if ( bb_is_set ( all_occupied_squares_bb, to_sq ) == false ) {
                        move_t quiet_move = move_encode_quiet ( from_sq, to_sq );
                        mvl_add ( mvl, quiet_move );
                }

                // capture moves
                const bitboard_t occ_mask = occ_mask_get_black_pawn_capture_non_first_double_move ( from_sq );
                bitboard_t capt_bb = white_bb & occ_mask;
                while ( capt_bb != 0 ) {
                        to_sq = bb_pop_1st_bit ( &capt_bb );
                        move_t capt_move = move_encode_capture ( from_sq, to_sq );
                        mvl_add ( mvl, capt_move );
                }

                if ( is_en_pass_active ) {
                        bitboard_t en_pass_bb;
                        bb_set_square ( &en_pass_bb, en_pass_sq );
                        if ( ( en_pass_bb & occ_mask ) != 0 ) {
                                // en passant move available from this square
                                move_t en_pass_move = move_encode_enpassant ( from_sq, en_pass_sq );
                                mvl_add ( mvl, en_pass_move );
                        }
                }

        }

        // now check for promotion
        bitboard_t pawns_on_rank2_bb = all_pawns_bb & RANK_2_BB;
        while ( pawns_on_rank2_bb != 0 ) {
                // quiet promotion
                from_sq = bb_pop_1st_bit ( &pawns_on_rank2_bb );
                to_sq = sq_get_square_minus_1_rank ( from_sq );
                if ( bb_is_set ( all_occupied_squares_bb, to_sq ) == false ) {
                        gen_promotions ( from_sq, to_sq, mvl, false, BLACK_PROMOTION_PIECES );
                }

                // capture promotion
                const bitboard_t occ_mask = occ_mask_get_black_pawn_capture_non_first_double_move ( from_sq );
                bitboard_t capt_bb = white_bb & occ_mask;
                while ( capt_bb != 0 ) {
                        to_sq = bb_pop_1st_bit ( &capt_bb );
                        gen_promotions ( from_sq, to_sq, mvl, true, BLACK_PROMOTION_PIECES );
                }
        }
}



static void gen_promotions ( const enum square from_sq, const enum square to_sq,
                             struct move_list *mvl, const bool is_capture, const enum piece *promotion_pieces )
{
        for ( int i = 0; i < NUM_PROMOTION_PIECES; i++ ) {
                move_t mv = move_encode_promoted ( from_sq, to_sq, *promotion_pieces++, is_capture );
                mvl_add ( mvl, mv );
        }
}



/**
 * @brief       Generates King moves
 * @param pos   The Position
 * @param side_to_move The side to move
 * @param mvl   the move list to append new moves to
 */
void mv_gen_king_moves ( const struct position *pos, const enum colour side_to_move, struct move_list *mvl )
{
        assert ( validate_position ( pos ) );
        assert ( validate_colour ( side_to_move ) );
        assert ( validate_move_list ( mvl ) );

        const struct board *brd = pos_get_board ( pos );
        enum piece pce_to_move;
        if ( side_to_move == WHITE ) {
                pce_to_move = WKING;
                mv_gen_white_castle_moves ( pos, mvl );
        } else {
                pce_to_move = BKING;
                mv_gen_black_castle_moves ( pos, mvl );
        }

        mv_gen_king_knight_moves ( brd, pce_to_move, side_to_move, mvl );
}



/**
 * @brief Generates moves for King and Knight (ie, non-sliding pieces)
 *
 * @param brd The board
 * @param pce_to_move The piece to move
 * @param side_to_move The side to move
 * @param mvl A pointer to the move List, to which the moves are appended.
 */
static void mv_gen_king_knight_moves ( const struct board *brd, const enum piece pce_to_move,
                                       const enum colour side_to_move,  struct move_list *mvl )
{
        // bitboard representing squares containing all pieces for the given colour
        bitboard_t bb = brd_get_piece_bb ( brd, pce_to_move );

        while ( bb != 0 ) {
                const enum square from_sq = bb_pop_1st_bit ( &bb );

                const bitboard_t occ_mask = occ_mask_get_piece_square ( pce_to_move, from_sq );

                // generate capture moves
                // ----------------------
                // AND'ing with opposite colour pieces with the occupancy mask, will
                // give all pieces that can be captured by the piece on this square
                const enum colour opp_side = swap_side ( side_to_move );
                const bitboard_t opp_colours_bb = brd_get_colour_bb ( brd, opp_side );
                const bitboard_t captures_bb = occ_mask & opp_colours_bb;
                mv_gen_encode_multiple_capture ( captures_bb, from_sq, mvl );

                // generate quiet moves
                // --------------------
                const bitboard_t all_occupied_squares_bb = brd_get_board_bb ( brd );
                const bitboard_t free_squares = ~all_occupied_squares_bb;
                const bitboard_t quiet_bb = free_squares & occ_mask;
                mv_gen_encode_multiple_quiet ( quiet_bb, from_sq, mvl );
        }
}





/**
 * @brief Generates quiet moves using the given bitboard and source square.
 *
 * @param bb A bitboard repesenting all possible non-occupied squares.
 * @param from_sq The source ("from") square
 * @param mvl Pointer to a move list. All generated moves are appended to this list.
 */
static void mv_gen_encode_multiple_quiet ( bitboard_t bb, const enum square from_sq, struct move_list *mvl )
{
        while ( bb != 0 ) {
                enum square empty_sq = bb_pop_1st_bit ( &bb );
                move_t quiet_move = move_encode_quiet ( from_sq, empty_sq );
                mvl_add ( mvl, quiet_move );
        }
}

/**
 * @brief Generates capture moves using the given bitboard and source square.
 *
 * @param bb A bitboard repesenting all possible capture squares.
 * @param from_sq The source ("from") square
 * @param mvl Pointer to a move list. All generated moves are appended to this list.
 */
static void mv_gen_encode_multiple_capture ( bitboard_t bb, const enum square from_sq, struct move_list *mvl )
{
        while ( bb != 0 ) {
                enum square cap_sq = bb_pop_1st_bit ( &bb );
                move_t cap_move = move_encode_capture ( from_sq, cap_sq );
                mvl_add ( mvl, cap_move );
        }
}

static void mv_gen_white_castle_moves ( const struct position *pos, struct move_list *mvl )
{
        cast_perm_t cp = pos_get_cast_perm ( pos );
        struct board *brd = pos_get_board ( pos );
        bitboard_t occupied_bb = brd_get_board_bb ( brd );

        if ( cast_perm_has_WK ( cp ) ) {
                add_kingside_move_if_no_blockers ( occupied_bb, CASTLE_MASK_WK, WHITE, mvl );
        }
        if ( cast_perm_has_WQ ( cp ) ) {
                add_queenside_move_if_no_blockers ( occupied_bb, CASTLE_MASK_WQ, WHITE, mvl );
        }
}


static void mv_gen_black_castle_moves ( const struct position *pos, struct move_list *mvl )
{
        cast_perm_t cp = pos_get_cast_perm ( pos );
        struct board *brd = pos_get_board ( pos );
        bitboard_t occupied_bb = brd_get_board_bb ( brd );

        if ( cast_perm_has_BK ( cp ) ) {
                add_kingside_move_if_no_blockers ( occupied_bb, CASTLE_MASK_BK, BLACK, mvl );
        }
        if ( cast_perm_has_BQ ( cp ) ) {
                add_queenside_move_if_no_blockers ( occupied_bb, CASTLE_MASK_BQ, BLACK, mvl );

        }
}

static void add_kingside_move_if_no_blockers ( const bitboard_t brd_bb, const bitboard_t blocking_pce_mask, const enum colour side_to_move, struct move_list *mvl )
{
        if ( ( brd_bb & blocking_pce_mask ) == 0 ) {
                move_t mv;
                if ( side_to_move == WHITE ) {
                        mv = move_encode_castle_kingside_white();
                } else {
                        mv = move_encode_castle_kingside_black();
                }
                mvl_add ( mvl, mv );
        }
}


static void add_queenside_move_if_no_blockers ( const bitboard_t brd_bb, const bitboard_t blocking_pce_mask, const enum colour side_to_move, struct move_list *mvl )
{
        if ( ( brd_bb & blocking_pce_mask ) == 0 ) {
                move_t mv;
                if ( side_to_move == WHITE ) {
                        mv = move_encode_castle_queenside_white();
                } else {
                        mv = move_encode_castle_queenside_black();
                }
                mvl_add ( mvl, mv );
        }
}



// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

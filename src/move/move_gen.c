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

#include <assert.h>
#include "move_gen.h"
#include "bitboard.h"
#include "board.h"
#include "move.h"
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


/**
 * @brief       Generates all valid moves for the given position
 * @details     Appends all new moves to the given move list
 * @param pos   The position
 * @param mvl   The move list
 */
void mv_gen_all_moves ( const struct position *pos, struct move_list *mvl )
{
        struct board *brd = pos_get_board ( pos );
        enum colour side_to_move = pos_get_side_to_move ( pos );

        mv_gen_knight_moves ( brd, side_to_move, mvl );
        mv_gen_diagonal_sliding_moves ( brd, side_to_move, mvl );
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
        validate_board ( brd );

        enum piece knight = WKNIGHT;
        if ( side_to_move == BLACK ) {
                knight = BKNIGHT;
        }

        // bitboard representing squares containing all Knights for the given colour
        bitboard_t knight_bb = brd_get_piece_bb ( brd, knight );

        while ( knight_bb != 0 ) {
                enum square knight_sq = bb_pop_1st_bit ( &knight_bb );

                bitboard_t occ_mask = get_knight_occ_mask ( knight_sq );

                // generate capture moves
                // ----------------------
                // AND'ing with opposite colour pieces with the occupancy mask, will
                // give all pieces that can be captured by the knight on this square
                enum colour opp_side = swap_side ( side_to_move );
                bitboard_t opp_colours_bb = brd_get_colour_bb ( brd, opp_side );
                bitboard_t captures_bb = occ_mask & opp_colours_bb;

                while ( captures_bb != 0 ) {
                        enum square cap_sq = bb_pop_1st_bit ( &captures_bb );
                        move_t cap_move = move_encode_capture ( knight_sq, cap_sq );
                        mvl_add ( mvl, cap_move );
                }

                // generate quiet moves
                // --------------------
                bitboard_t all_occupied_squares_bb = brd_get_board_bb ( brd );
                bitboard_t free_squares = ~all_occupied_squares_bb;
                bitboard_t quiet_move_squares = free_squares & occ_mask;
                while ( quiet_move_squares != 0 ) {
                        enum square empty_sq = bb_pop_1st_bit ( &quiet_move_squares );
                        move_t quiet_move = move_encode_quiet ( knight_sq, empty_sq );
                        mvl_add ( mvl, quiet_move );
                }
        }
}

/**
 * @brief       Generates diagonal sliding moves (Queen and Bishop moves)
 * @details     Generates all valid diagonal moves for Bishop and Queen of the
 * given colour. New moves are appended to the given move list.
 * Based on the code on page:
 * 		http://chessprogramming.wikispaces.com/Efficient+Generation+of+Sliding+Piece+Attacks
 * @param brd   The board
 * @param side_to_move The side to move
 * @param mvl   the move list to append new moves to
 */
void mv_gen_diagonal_sliding_moves ( const struct board *brd, const enum colour side_to_move, struct move_list *mvl )
{
        validate_board ( brd );

        enum piece pce_bishop;
        enum piece pce_queen;

        if ( side_to_move == WHITE ) {
                pce_bishop = WBISHOP;
                pce_queen = WQUEEN;

        }       else {
                pce_bishop = BBISHOP;
                pce_queen = BQUEEN;
        }

        // bitboard representing squares containing all Bishops and Queens
        bitboard_t combined_bb = brd_get_piece_bb ( brd, pce_bishop ) | brd_get_piece_bb ( brd, pce_queen );

        while ( combined_bb != 0 ) {
                enum square from_sq = bb_pop_1st_bit ( &combined_bb );

                bitboard_t posmask = GET_DIAGONAL_MASK ( from_sq );
                bitboard_t negmask = GET_ANTI_DIAGONAL_MASK ( from_sq );

                // create slider bb for this square
                bitboard_t bb_slider;
                bb_set_square ( &bb_slider, from_sq );

                // all occupied squares (both colours)
                bitboard_t occupied = brd_get_board_bb ( brd );

                bitboard_t diag1 = ( occupied & posmask ) - ( 2 * bb_slider );
                bitboard_t diag2 = bb_reverse (
                                           bb_reverse ( occupied & posmask ) -
                                           ( 2 * bb_reverse ( bb_slider ) ) );
                bitboard_t diagpos = diag1 ^ diag2;

                diag1 = ( occupied & negmask ) - ( 2 * bb_slider );
                diag2 = bb_reverse ( bb_reverse ( occupied & negmask ) -
                                          ( 2 * bb_reverse ( bb_slider ) ) );
                bitboard_t diagneg = diag1 ^ diag2;

                bitboard_t all_moves = ( diagpos & posmask ) | ( diagneg & negmask );

                // get all same colour as piece being considered
                bitboard_t col_occupied = brd_get_colour_bb ( brd, side_to_move );
                bitboard_t excl_same_col = all_moves & ~col_occupied;

                while ( excl_same_col != 0 ) {
                        enum square to_sq = bb_pop_1st_bit ( &excl_same_col );
                        move_t mv;
                        bool found = brd_is_sq_occupied ( brd, to_sq );
                        if ( found ) {
                                mv = move_encode_capture ( from_sq, to_sq );
                        } else {
                                mv = move_encode_quiet ( from_sq, to_sq );
                        }
                        mvl_add ( mvl, mv );
                }
        }
}



/**
 * @brief       Generates horizontal and vertical sliding moves (Queen and Rook moves)
 * @details     Generates all valid vertical and horizontal moves for Rook and Queen of the
 * given colour. New moves are appended to the given move list.
 * Based on the code on page:
 * 		http://chessprogramming.wikispaces.com/Efficient+Generation+of+Sliding+Piece+Attacks
 * @param brd   The board
 * @param side_to_move The side to move
 * @param mvl   the move list to append new moves to
 */
void mv_gen_sliding_horizontal_vertical_moves ( const struct board *brd, const enum colour side_to_move, struct move_list *mvl )
{
        validate_board ( brd );

        enum piece pce_rook;
        enum piece pce_queen;

        if ( side_to_move == WHITE ) {
                pce_rook = WROOK;
                pce_queen = WQUEEN;

        }       else {
                pce_rook = BROOK;
                pce_queen = BQUEEN;
        }

        // bitboard representing squares containing all Rooks and Queens
        bitboard_t combined_bb = brd_get_piece_bb ( brd, pce_rook ) | brd_get_piece_bb ( brd, pce_queen );

        while ( combined_bb != 0 ) {

                enum square from_sq = bb_pop_1st_bit ( &combined_bb );

                bitboard_t hmask = GET_HORIZONTAL_MASK ( from_sq );
                bitboard_t vmask = GET_VERTICAL_MASK ( from_sq );

                bitboard_t bb_slider;
                bb_set_square ( &bb_slider, from_sq );

                // all occupied squares (both colours)
                bitboard_t occupied = brd_get_board_bb ( brd );

                uint64_t horiz1 = occupied - ( 2 * bb_slider );
                uint64_t horiz2 =
                        bb_reverse ( bb_reverse ( occupied ) -
                                          2 * bb_reverse ( bb_slider ) );
                uint64_t horizontal = horiz1 ^ horiz2;

                uint64_t vert1 = ( occupied & vmask ) - ( 2 * bb_slider );
                uint64_t vert2 =
                        bb_reverse ( bb_reverse ( occupied & vmask ) -
                                          2 * bb_reverse ( bb_slider ) );
                uint64_t vertical = vert1 ^ vert2;

                uint64_t all_moves = ( horizontal & hmask ) | ( vertical & vmask );

                // get all same colour as piece being considered so
                // they can be excluded (since they block a move)
                uint64_t col_occupied = brd_get_colour_bb ( brd, side_to_move );
                uint64_t excl_same_col = all_moves & ~col_occupied;

                while ( excl_same_col != 0 ) {
                        enum square to_sq = bb_pop_1st_bit ( &excl_same_col );
                        bool found = brd_is_sq_occupied ( brd, to_sq );
                        move_t mv;
                        if ( found ) {
                                mv = move_encode_capture ( from_sq, to_sq );
                        } else {
                                mv = move_encode_quiet ( from_sq, to_sq );
                        }
                        mvl_add ( mvl, mv );
                }
        }
}




// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

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

#include "move_gen.h"
#include "bitboard.h"
#include "board.h"
#include "move_list.h"
#include "occupancy_mask.h"
#include "piece.h"
#include "position.h"

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
        validate_board (brd);

        
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


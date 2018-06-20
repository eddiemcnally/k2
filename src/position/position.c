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


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "position.h"
#include "board.h"
#include "fen.h"
#include "board.h"
#include "move.h"
#include "castle_perms.h"

static void init_pos_struct ( struct position *pos );
static void populate_position_from_fen ( struct position *pos, const struct parsed_fen *fen );
static void set_up_castle_permissions ( struct position *pos, const struct parsed_fen *fen );
static bool validate_en_passant_pce_and_sq ( const struct position *pos );


// key used to verify struct has been initialised
const static uint16_t STRUCT_INIT_KEY = 0xdead;

#define MAX_GAME_MOVES          (1024)

struct mv_undo {
        uint16_t        move;
        uint8_t         fifty_move_counter;
        uint8_t         castle_perm;
        uint64_t        board_hash;
        enum square     en_passant;
        bool            en_pass_set;
};



struct position {
        uint16_t struct_init_key;

        // current board representation
        struct board *brd;

        // the next side to move
        enum colour side_to_move;

        // the square where en passent is active
        enum square en_passant;
        bool en_passant_set;

        // keeping track of ply
        uint16_t ply;         // half-moves
        uint16_t history_ply;

        // castling permissions
        uint8_t castle_perm;

        uint8_t fifty_move_counter;

        // move history
        struct mv_undo history[MAX_GAME_MOVES];

};






/**
 * @brief       Create and initialise an empty instance of the Position struct
 *
 * @return      An initialised Position struct
 */
struct position * pos_create()
{
        struct position *retval = ( struct position * ) malloc ( sizeof ( struct position ) );
        init_pos_struct ( retval );

        struct board *brd = brd_allocate();
        retval->brd = brd;

        return retval;
}


/**
 * @brief       Sets up the position using the goven FEN
 *
 * @param fen   The FEN string
 * @param pos   The position struct
 */
void pos_initialise ( const char * fen, struct position *pos )
{
        struct parsed_fen *parsed_fen = fen_parse ( fen );

        populate_position_from_fen ( pos, parsed_fen );
}


/**
 * @brief       Cleans up the Position and frees up any memory
 *
 * @param pos   The position to destroy
 */
void pos_destroy ( struct position *pos )
{
        assert ( validate_position ( pos ) );
        brd_deallocate ( pos->brd );

        memset ( pos, 0, sizeof ( struct position ) );
        free ( pos );
}

/**
 * @brief       Returns the board managed by this position
 *
 * @param pos   The Position
 * @return      The board
 */
struct board * pos_get_board ( const struct position *pos )
{
        assert ( validate_position ( pos ) );
        return pos->brd;
}

/**
 * @brief       Gets the side to move from the position struct
 *
 * @param pos   The position
 * @return Side to move
 */
enum colour pos_get_side_to_move ( const struct position *pos )
{
        assert ( validate_position ( pos ) );
        return pos->side_to_move;
}

uint8_t pos_get_cast_perm ( const struct position *pos )
{
        return pos->castle_perm;
}


bool pos_try_get_en_pass_sq ( const struct position *pos, enum square *en_pass_sq )
{
        if ( pos->en_passant_set ) {
                *en_pass_sq = pos->en_passant;
                return true;
        }
        return false;
}



void pos_set_cast_perm ( struct position *pos, const uint8_t perms )
{
        pos->castle_perm = perms;
}


bool validate_position ( const struct position *pos )
{
        assert ( pos->struct_init_key == STRUCT_INIT_KEY );

        assert ( validate_board ( pos->brd ) );
        assert ( validate_colour ( pos->side_to_move ) );

        return true;
}


bool pos_try_make_move ( struct position *pos, const uint16_t mv )
{
        assert ( validate_position ( pos ) );

        pos->ply++;

        const enum square from_sq = move_decode_from_sq ( mv );
        const enum square to_sq = move_decode_to_sq ( mv );
        enum piece pce_to_move;
        bool found = brd_try_get_piece_on_square ( pos->brd, to_sq, &pce_to_move );
        assert ( found == true );

        if ( move_is_quiet ( mv ) ) {
                brd_move_piece ( pos->brd, pce_to_move, from_sq, to_sq );
        } else {
                if ( move_is_capture ( mv ) ) {
                        enum piece pce_capt;
                        if ( move_is_en_passant ( mv ) ) {
                                assert ( validate_en_passant_pce_and_sq ( pos ) );
                                enum square en_pass_pce_sq;

                                // flip the flag
                                pos->en_passant_set = false;

                                if ( pos->side_to_move == WHITE ) {
                                        pce_capt = BPAWN;
                                        en_pass_pce_sq = sq_get_square_minus_1_rank ( to_sq );
                                } else {
                                        pce_capt = WPAWN;
                                        en_pass_pce_sq = sq_get_square_plus_1_rank ( to_sq );
                                }
                                brd_remove_piece ( pos->brd, pce_capt, en_pass_pce_sq );

                        } else {
                                found = brd_try_get_piece_on_square ( pos->brd, to_sq, &pce_capt );
                                assert ( found == true );

                                brd_remove_piece ( pos->brd, pce_capt, to_sq );
                        }
                        brd_move_piece ( pos->brd, pce_to_move, from_sq, to_sq );
                }

                if ( move_is_promotion ( mv ) ) {
                        enum piece pce_prom = move_decode_promotion_piece ( mv, pos->side_to_move );
                        brd_move_piece ( pos->brd, pce_to_move, from_sq, to_sq );
                        brd_remove_piece ( pos->brd, pce_to_move, to_sq );
                        brd_add_piece ( pos->brd, pce_prom, to_sq );
                }
        }
        return true;
}


uint16_t pos_take_move ( struct position *pos )
{
        assert ( validate_position ( pos ) );

        if ( pos->side_to_move == WHITE ) {
                pos->ply--;
        }

        return ( uint16_t ) 0;
}




static void init_pos_struct ( struct position *pos )
{
        memset ( pos, 0, sizeof ( struct position ) );
        pos->struct_init_key = STRUCT_INIT_KEY;
}


static void populate_position_from_fen ( struct position *pos, const struct parsed_fen *fen )
{
        pos->side_to_move = fen_get_side_to_move ( fen );

        enum square en_pass;
        bool found_en_pass = fen_try_get_en_pass_sq ( fen, &en_pass );
        if ( found_en_pass ) {
                pos->en_passant_set = true;
                pos->en_passant = en_pass;
        } else {
                pos->en_passant_set = false;
        }

        pos->fifty_move_counter = 0;
        pos->ply = fen_get_half_move_cnt ( fen );
        pos->history_ply = fen_get_full_move_cnt ( fen );
        set_up_castle_permissions ( pos, fen );

        for ( enum square sq = a1; sq <= h8; sq++ ) {
                enum piece pce;
                bool found_pce = fen_try_get_piece_on_sq ( fen, sq, &pce );
                if ( found_pce ) {
                        brd_add_piece ( pos->brd, pce, sq );
                }
        }
}


static bool validate_en_passant_pce_and_sq ( const struct position *pos )
{
        assert ( pos->en_passant_set == true );
        enum piece en_pass_pce;
        bool found = brd_try_get_piece_on_square ( pos->brd, pos->en_passant, &en_pass_pce );
        assert ( found == true );
        assert ( en_pass_pce == WPAWN || en_pass_pce == BPAWN );
        return true;
}


static void set_up_castle_permissions ( struct position *pos, const struct parsed_fen *fen )
{
        uint8_t cp;
        // default to no castle permissions
        cast_perm_set_no_perms ( &cp );

        if ( fen_has_wk_castle_perms ( fen ) ) {
                cast_perm_set_WK ( &cp, true );
        }
        if ( fen_has_wq_castle_perms ( fen ) ) {
                cast_perm_set_WQ ( &cp, true );
        }
        if ( fen_has_bk_castle_perms ( fen ) ) {
                cast_perm_set_BK ( &cp, true );
        }
        if ( fen_has_bq_castle_perms ( fen ) ) {
                cast_perm_set_BQ ( &cp, true );
        }

        pos_set_cast_perm ( pos, cp );

}


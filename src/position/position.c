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
#include "fen.h"
#include "board.h"
#include "castle_perms.h"

static void init_pos_struct ( struct position *pos );
static void populate_position_from_fen ( struct position *pos, const struct parsed_fen *fen );
static void set_up_castle_permissions ( struct position *pos, const struct parsed_fen *fen );

#define STRUCT_INIT_KEY     ((uint64_t)0xdeadbeef)


struct position {
        uint64_t struct_init_key;

        // current board representation
        struct board *brd;

        // the next side to move
        enum colour side_to_move;

        // a hash of the current board
        uint64_t board_hash;

        // the square where en passent is active
        enum square en_passant;
        bool en_passant_set;

        // keeping track of ply
        uint16_t ply;
        uint16_t history_ply;

        // castling permissions
        cast_perm_t castle_perm;

        uint8_t fifty_move_counter;
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

cast_perm_t pos_get_cast_perm ( const struct position *pos )
{
        return pos->castle_perm;
}


void pos_set_cast_perm ( struct position *pos, const cast_perm_t perms )
{
        pos->castle_perm = perms;
}


bool validate_position ( const struct position *pos )
{
        if ( pos->struct_init_key != STRUCT_INIT_KEY ) {
                return false;
        }

        assert ( validate_board ( pos->brd ) );
        assert ( validate_colour ( pos->side_to_move ) );

        return true;

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


static void set_up_castle_permissions ( struct position *pos, const struct parsed_fen *fen )
{
        cast_perm_t cp;
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


// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 

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
#include <stdint.h>
#include <stdio.h>
#include "move.h"
#include "piece.h"
#include "square.h"

/**
 * bitmap for move
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

enum move_bits_masks {
        MV_MASK_TO_SQ = 0x003F,
        MV_MASK_FROM_SQ = 0x0FC0,
        MV_MASK_FLAGS = 0xF000,
};

enum move_bits_shifts {
        MV_SHFT_TO_SQ = 0,
        MV_SHFT_FROM_SQ = 6,
};

enum move_flags {
        MV_FLG_QUIET = 0x0000,
        MV_FLG_DOUBLE_PAWN = 0x1000,
        MV_FLG_KING_CASTLE = 0x2000,
        MV_FLG_QUEEN_CASTLE = 0x3000,
        MV_FLG_CAPTURE = 0x4000,
        MV_FLG_EN_PASS = 0x5000,
        MV_FLG_PROMOTE_KNIGHT = 0x8000,
        MV_FLG_PROMOTE_BISHOP = 0x9000,
        MV_FLG_PROMOTE_ROOK = 0xA000,
        MV_FLG_PROMOTE_QUEEN = 0xB000,
        MV_FLG_PROMOTE_KNIGHT_CAPTURE = 0xC000,
        MV_FLG_PROMOTE_BISHOP_CAPTURE = 0xD000,
        MV_FLG_PROMOTE_ROOK_CAPTURE = 0xE000,
        MV_FLG_PROMOTE_QUEEN_CAPTURE = 0xF000,
};

enum move_flag_bits {
        MV_FLG_BIT_PROMOTE = 0x8000,
        MV_FLG_BIT_CAPTURE = 0x4000,
};

static void set_flag ( move_t *mv, const uint16_t flag );
static move_t encode_to_from ( const enum square from_sq,
                               const enum square to_sq );

// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief           Encodes a quiet move using the given to and from squares
 *
 * @param from_sq   The from square
 * @param to_sq     The to square
 * @return The encoded move
 */
move_t move_encode_quiet ( const enum square from_sq, const enum square to_sq )
{
        return encode_to_from ( from_sq, to_sq );
}

/**
 * @brief           Encodes a promotion move, including a possibly capture
 *
 * @param from_sq   The from square
 * @param to_sq     The to square
 * @param promoted_piece The target promotion piece
 * @param is_capture true if also a capture move, false otherwise
 * @return The encoded move
 */
move_t move_encode_promoted ( const enum square from_sq, const enum square to_sq,
                              const enum piece promoted_piece,
                              const bool is_capture )
{
        move_t mv = encode_to_from ( from_sq, to_sq );

        enum piece pce_type = pce_get_piece_type ( promoted_piece );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

        switch ( pce_type ) {
        case KNIGHT:
                set_flag ( &mv, MV_FLG_PROMOTE_KNIGHT );
                break;

        case BISHOP:
                set_flag ( &mv, MV_FLG_PROMOTE_BISHOP );
                break;

        case ROOK:
                set_flag ( &mv, MV_FLG_PROMOTE_ROOK );
                break;

        case QUEEN:
                set_flag ( &mv, MV_FLG_PROMOTE_QUEEN );
                break;

        default:
                assert ( false );
                break;
        }

#pragma GCC diagnostic pop

        if ( is_capture ) {
                set_flag ( &mv, MV_FLG_BIT_CAPTURE );
        }

        return mv;
}

/**
 * @brief       Encodes a capture move using the given squares
 *
 * @param from_sq The from square
 * @param to_sq The to square
 * @return The encoded move
 */
move_t move_encode_capture ( const enum square from_sq, const enum square to_sq )
{
        move_t mv = encode_to_from ( from_sq, to_sq );
        set_flag ( &mv, MV_FLG_CAPTURE );
        return mv;
}

/**
 * @brief       Encodes an En Passant move using the given squares
 *
 * @param from_sq The from square
 * @param to_sq The to square
 * @return The encoded move
 */
move_t move_encode_enpassant ( const enum square from_sq,
                               const enum square to_sq )
{
        move_t mv = encode_to_from ( from_sq, to_sq );
        set_flag ( &mv, MV_FLG_EN_PASS );
        return mv;
}

/**
 * @brief       Extracts the from square from the given move
 *
 * @param mv The move
 * @return The from square
 */
enum square move_decode_from_sq ( const move_t mv )
{
        return ( enum square ) ( ( mv & MV_MASK_FROM_SQ ) >> MV_SHFT_FROM_SQ );
}

/**
 * @brief       Extracts the to square from the given move
 *
 * @param mv The move
 * @return The to square
 */
enum square move_decode_to_sq ( const move_t mv )
{
        return ( enum square ) ( ( mv & MV_MASK_TO_SQ ) >> MV_SHFT_TO_SQ );
}

/**
 * @brief       Tests the given move_t, returns true if Quiet, false otherwise
 *
 * @param mv The move to test
 * @return true if quiet, false otherwise
 */
bool move_is_quiet ( move_t mv )
{
        move_t m = ( ( move_t ) mv ) & MV_MASK_FLAGS;
        return m == MV_FLG_QUIET;
}

/**
 * @brief       Tests the given move_t, returns true if a Capture move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if Capture, false otherwise
 */
bool move_is_capture ( move_t mv )
{
        return ( mv & MV_FLG_BIT_CAPTURE ) != 0;
}

/**
 * @brief       Tests the given move_t, returns true if a Promotion move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if a promotion, false otherwise
 */
bool move_is_promotion ( move_t mv )
{
        return ( mv & MV_FLG_BIT_PROMOTE ) != 0;
}

/**
 * @brief       Tests the given move_t, returns true if an En Passant move,
 * false otherwise
 *
 * @param mv The move to test
 * @return true if an En Passant move, false otherwise
 */
bool move_is_en_passant ( move_t mv )
{
        return ( mv & MV_FLG_EN_PASS ) != 0;
}



/**
 * @brief       Prints a move
 *
 * @param mv The move print
 * @return The move in test
 */
char *move_print ( move_t mv )
{
        static char move_string[6];

        enum square from_sq = move_decode_from_sq ( mv );
        enum square to_sq = move_decode_to_sq ( mv );

        enum rank from_rank = sq_get_rank ( from_sq );
        enum file from_file = sq_get_file ( from_sq );

        enum rank to_rank = sq_get_rank ( to_sq );
        enum file to_file = sq_get_file ( to_sq );

        sprintf ( move_string, "%c%c%c%c", ( 'a' + from_file ),
                  ( '1' + from_rank ), ( 'a' + to_file ), ( '1' + to_rank ) );

        return move_string;
}




// ==================================================================
//
// private functions
//
// ==================================================================

static move_t encode_to_from ( const enum square from_sq,
                               const enum square to_sq )
{
        move_t mv = 0;
        uint16_t m = ( uint16_t ) ( from_sq << MV_SHFT_FROM_SQ );
        mv |= ( uint16_t ) ( m & MV_MASK_FROM_SQ );

        m = ( uint16_t ) ( to_sq << MV_SHFT_TO_SQ );
        mv |= ( uint16_t ) ( m & MV_MASK_TO_SQ );

        return mv;
}

static void set_flag ( move_t *mv, const uint16_t flag_bit )
{
        *mv |= flag_bit;
}
// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 
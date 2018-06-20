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
#include <stdbool.h>
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
        MV_MASK_TO_SQ   = 0x003F,
        MV_MASK_FROM_SQ = 0x0FC0,
        MV_MASK_FLAGS   = 0xF000,
};

enum move_bits_shifts {
        MV_SHFT_TO_SQ   = 0,
        MV_SHFT_FROM_SQ = 6,
};

enum move_flags {
        MV_FLG_QUIET                    = 0x0000,
        MV_FLG_DOUBLE_PAWN              = 0x1000,
        MV_FLG_KING_CASTLE              = 0x2000,
        MV_FLG_QUEEN_CASTLE             = 0x3000,
        MV_FLG_CAPTURE                  = 0x4000,
        MV_FLG_EN_PASS                  = 0x5000,
        MV_FLG_PROMOTE_KNIGHT           = 0x8000,
        MV_FLG_PROMOTE_BISHOP           = 0x9000,
        MV_FLG_PROMOTE_ROOK             = 0xA000,
        MV_FLG_PROMOTE_QUEEN            = 0xB000,
        MV_FLG_PROMOTE_KNIGHT_CAPTURE   = 0xC000,
        MV_FLG_PROMOTE_BISHOP_CAPTURE   = 0xD000,
        MV_FLG_PROMOTE_ROOK_CAPTURE     = 0xE000,
        MV_FLG_PROMOTE_QUEEN_CAPTURE    = 0xF000,
};

enum move_flag_bits {
        MV_FLG_BIT_PROMOTE = 0x8000,
        MV_FLG_BIT_CAPTURE = 0x4000,
};

static void set_flag ( uint16_t *mv, const uint16_t flag );
static uint16_t encode_to_from ( const enum square from_sq,
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
uint16_t move_encode_quiet ( const enum square from_sq, const enum square to_sq )
{
        assert ( validate_square ( from_sq ) );
        assert ( validate_square ( to_sq ) );

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
uint16_t move_encode_promoted ( const enum square from_sq, const enum square to_sq,
                                const enum piece promoted_piece,
                                const bool is_capture )
{
        assert ( validate_square ( from_sq ) );
        assert ( validate_square ( to_sq ) );
        assert ( validate_piece ( promoted_piece ) );

        uint16_t mv = encode_to_from ( from_sq, to_sq );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

        // TODO : benchmark this switch statement. Possibly replace with an
        // array lookup of the flag
        switch ( promoted_piece ) {
        case WKNIGHT:
        case BKNIGHT:
                mv |= MV_FLG_PROMOTE_KNIGHT;
                break;

        case WBISHOP:
        case BBISHOP:
                mv |= MV_FLG_PROMOTE_BISHOP;
                break;

        case WROOK:
        case BROOK:
                mv |= MV_FLG_PROMOTE_ROOK;
                break;

        case WQUEEN:
        case BQUEEN:
                mv |= MV_FLG_PROMOTE_QUEEN;
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
 * @brief       Extracts the piece class from a promotion move
 *
 * @param mv    The move
 * @return      The piece
 */
enum piece move_decode_promotion_piece ( const uint16_t mv , const enum colour side )
{
        assert ( validate_move ( mv ) );

        const uint16_t m = mv & MV_MASK_FLAGS;

        switch ( m ) {
        case MV_FLG_PROMOTE_KNIGHT_CAPTURE:
        case MV_FLG_PROMOTE_KNIGHT:
                if ( side == WHITE ) return WKNIGHT;
                return BKNIGHT;
        case MV_FLG_PROMOTE_BISHOP_CAPTURE:
        case MV_FLG_PROMOTE_BISHOP:
                if ( side == WHITE ) return WBISHOP;
                return BBISHOP;
        case MV_FLG_PROMOTE_QUEEN_CAPTURE:
        case MV_FLG_PROMOTE_QUEEN:
                if ( side == WHITE ) return WQUEEN;
                return BQUEEN;
        case MV_FLG_PROMOTE_ROOK_CAPTURE:
        case MV_FLG_PROMOTE_ROOK:
                if ( side == WHITE ) return WROOK;
                return BROOK;
        default:
                assert ( false );
        }
}



/**
 * @brief       Encodes a capture move using the given squares
 *
 * @param from_sq The from square
 * @param to_sq The to square
 * @return The encoded move
 */
uint16_t move_encode_capture ( const enum square from_sq, const enum square to_sq )
{
        assert ( validate_square ( from_sq ) );
        assert ( validate_square ( to_sq ) );

        uint16_t mv = encode_to_from ( from_sq, to_sq );
        set_flag ( &mv, MV_FLG_CAPTURE );
        return mv;
}

/**
 * @brief       Encodes a WK castle move
 *
 * @return      The encoded move
 */
uint16_t move_encode_castle_kingside_white ( void )
{
        uint16_t mv = encode_to_from ( e1, g1 );
        set_flag ( &mv, MV_FLG_KING_CASTLE );
        return mv;
}

/**
 * @brief       Encodes a BK castle move
 *
 * @return      The encoded move
 */
uint16_t move_encode_castle_kingside_black ( void )
{
        uint16_t mv = encode_to_from ( e8, g8 );
        set_flag ( &mv, MV_FLG_KING_CASTLE );
        return mv;
}

/**
 * @brief       Encodes a WQ castle move
 *
 * @return      The encoded move
 */
uint16_t move_encode_castle_queenside_white ( void )
{
        uint16_t mv = encode_to_from ( e1, c1 );
        set_flag ( &mv, MV_FLG_QUEEN_CASTLE );
        return mv;
}

/**
 * @brief       Encodes a BQ castle move
 *
 * @return      The encoded move
 */
uint16_t move_encode_castle_queenside_black ( void )
{
        uint16_t mv = encode_to_from ( e8, c8 );
        set_flag ( &mv, MV_FLG_QUEEN_CASTLE );
        return mv;
}

/**
 * @brief       Encodes a double first move for a pawn
 * @param       from_sq the from square
 * @param       to_sq the to square
 * @return      The encoded move
 */
uint16_t move_encode_pawn_double_first ( const enum square from_sq, const enum square to_sq )
{
        assert ( validate_square ( from_sq ) );
        assert ( validate_square ( to_sq ) );

        uint16_t mv = encode_to_from ( from_sq, to_sq );
        set_flag ( &mv, MV_FLG_DOUBLE_PAWN );
        return mv;
}


/**
 * @brief       Encodes an En Passant move using the given squares
 *
 * @param from_sq The from square
 * @param to_sq The to square
 * @return The encoded move
 */
uint16_t move_encode_enpassant ( const enum square from_sq,
                                 const enum square to_sq )
{
        assert ( validate_square ( from_sq ) );
        assert ( validate_square ( to_sq ) );

        uint16_t mv = encode_to_from ( from_sq, to_sq );
        set_flag ( &mv, MV_FLG_EN_PASS );
        return mv;
}

/**
 * @brief       Extracts the from square from the given move
 *
 * @param mv The move
 * @return The from square
 */
enum square move_decode_from_sq ( const uint16_t mv )
{
        return ( enum square ) ( ( mv & MV_MASK_FROM_SQ ) >> MV_SHFT_FROM_SQ );
}

/**
 * @brief       Extracts the to square from the given move
 *
 * @param mv The move
 * @return The to square
 */
enum square move_decode_to_sq ( const uint16_t mv )
{
        return ( enum square ) ( ( mv & MV_MASK_TO_SQ ) >> MV_SHFT_TO_SQ );
}



/**
 * @brief       Tests the given uint16_t, returns true if Quiet, false otherwise
 *
 * @param mv The move to test
 * @return true if quiet, false otherwise
 */
bool move_is_quiet ( const uint16_t mv )
{
        uint16_t m = ( ( uint16_t ) mv ) & MV_MASK_FLAGS;
        return m == MV_FLG_QUIET;
}

/**
 * @brief       Tests the given uint16_t, returns true if a Capture move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if Capture, false otherwise
 */
bool move_is_capture ( const uint16_t mv )
{
        assert ( validate_move ( mv ) );

        return ( mv & MV_FLG_BIT_CAPTURE ) != 0;
}

/**
 * @brief       Tests the given uint16_t, returns true if a Promotion move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if a promotion, false otherwise
 */
bool move_is_promotion ( const uint16_t mv )
{
        assert ( validate_move ( mv ) );

        return ( mv & MV_FLG_BIT_PROMOTE ) != 0;
}

/**
 * @brief       Tests the given uint16_t, returns true if an En Passant move,
 * false otherwise
 *
 * @param mv The move to test
 * @return true if an En Passant move, false otherwise
 */
bool move_is_en_passant ( const uint16_t mv )
{
        assert ( validate_move ( mv ) );

        return ( mv & MV_FLG_EN_PASS ) != 0;
}



/**
 * @brief       Prints a move
 *
 * @param mv The move print
 * @return The move in test
 */
char *move_print ( const uint16_t mv )
{
        assert ( validate_move ( mv ) );

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


bool validate_move ( const uint16_t mv )
{
        enum square from = move_decode_from_sq ( mv );
        enum square to = move_decode_to_sq ( mv );
        bool from_ok = validate_square ( from );
        bool to_ok = validate_square ( to );
        if ( from_ok && to_ok ) {
                return true;
        }

        assert ( false );
}

// ==================================================================
//
// private functions
//
// ==================================================================

static uint16_t encode_to_from ( const enum square from_sq,
                                 const enum square to_sq )
{
        uint16_t mv = 0;

        uint16_t m = ( uint16_t ) ( from_sq << MV_SHFT_FROM_SQ );
        mv |= ( uint16_t ) ( m & MV_MASK_FROM_SQ );

        m = ( uint16_t ) ( to_sq << MV_SHFT_TO_SQ );
        mv |= ( uint16_t ) ( m & MV_MASK_TO_SQ );

        return mv;
}

static void set_flag ( uint16_t *mv, const uint16_t flag_bit )
{
        *mv |= flag_bit;
}



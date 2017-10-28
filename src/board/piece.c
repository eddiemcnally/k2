#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "piece.h"

/**
 * Piece values
 */
enum piece_values {
    PCE_VAL_PAWN = 100,
    PCE_VAL_BISHOP = 325,
    PCE_VAL_KNIGHT = 325,
    PCE_VAL_ROOK = 500,
    PCE_VAL_QUEEN = 1000,
    PCE_VAL_KING = 50000
};

#define     COLOUR_MASK     0x03


bool pce_is_white(const enum piece pce)
{
    validate_piece(pce);
    return ((pce & WHITE) != 0);
}

bool pce_is_black(const enum piece pce)
{
    validate_piece(pce);
    return ((pce & BLACK) != 0);
}

enum colour swap_side(const enum colour col)
{
    validate_colour(col);

    if (col == WHITE) {
        return BLACK;
    }
    return WHITE;
}

enum colour pce_get_colour(const enum piece pce)
{
    validate_piece(pce);

    if (pce_is_white(pce)) {
        return WHITE;
    }
    return BLACK;
}

uint32_t pce_get_value(const enum piece pce)
{
    validate_piece(pce);

    enum piece p = pce_get_piece_type(pce);

    switch (p) {
    case PAWN:
        return PCE_VAL_PAWN;
    case BISHOP:
        return PCE_VAL_BISHOP;
    case KNIGHT:
        return PCE_VAL_KNIGHT;
    case ROOK:
        return PCE_VAL_ROOK;
    case QUEEN:
        return PCE_VAL_QUEEN;
    case KING:
        return PCE_VAL_KING;
    default:
        assert(false);
        break;
    }
}

char pce_get_label(const enum piece pce)
{
    validate_piece(pce);

    switch (pce) {
    case WPAWN:
        return 'P';
    case BPAWN:
        return 'p';
    case WBISHOP:
        return 'B';
    case BBISHOP:
        return 'b';
    case WKNIGHT:
        return 'N';
    case BKNIGHT:
        return 'n';
    case WROOK:
        return 'R';
    case BROOK:
        return 'r';
    case WQUEEN:
        return 'Q';
    case BQUEEN:
        return 'q';
    case WKING:
        return 'K';
    case BKING:
        return 'k';
    default:
        assert(false);
        return '-';

    }
}

enum piece pce_get_from_label(const char c)
{
    validate_label(c);
    switch (c) {
    case 'p':
        return BPAWN;
    case 'r':
        return BROOK;
    case 'n':
        return BKNIGHT;
    case 'b':
        return BBISHOP;
    case 'q':
        return BQUEEN;
    case 'k':
        return BKING;
    case 'P':
        return WPAWN;
    case 'R':
        return WROOK;
    case 'N':
        return WKNIGHT;
    case 'B':
        return WBISHOP;
    case 'Q':
        return WQUEEN;
    case 'K':
        return WKING;
    default:
        assert(false);
    }
}

enum piece pce_get_piece_type(const enum piece piece)
{
    return (enum piece)(piece & (uint8_t) (~COLOUR_MASK));
}

void validate_piece(const enum piece pce)
{
#ifdef ENABLE_ASSERTS
    switch (pce) {
    case WPAWN:
        case BPAWN:
            case WKNIGHT:
                case BKNIGHT:
                    case WBISHOP:
                        case BBISHOP:
                            case WROOK:
                                case BROOK:
                                    case WQUEEN:
                                        case BQUEEN:
                                            case WKING:
                                                case BKING:
                                                        assert(true);
        break;
    default:
        assert(false);
        break;
    }
#endif
}

void validate_colour(const enum colour col)
{
#ifdef ENABLE_ASSERTS
    assert(col == WHITE || col == BLACK);
#endif

}

void validate_label(const char c)
{
#ifdef ENABLE_ASSERTS
    switch (c) {
    case 'p':
    case 'r':
    case 'n':
    case 'b':
    case 'q':
    case 'k':
    case 'P':
    case 'R':
    case 'N':
    case 'B':
    case 'Q':
    case 'K':
        assert(true);
        break;
    default:
        printf("Invalid label %c\n", c);
        assert(false);
    }
#endif
}

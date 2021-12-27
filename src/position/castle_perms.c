/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without 
 *  restriction, including without limitation the rights to use, 
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following 
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*! @addtogroup CastlePermission
 *
 * @ingroup CastlePermissions
 * @{
 * @details Code associated with encoding and decoding castle permissions
 *
 */

#include "castle_perms.h"
#include "utils.h"
#include <assert.h>

enum cast_perm_bitmap {
    CAST_PERM_WK = (0x01 << 0),
    CAST_PERM_WQ = (0x01 << 1),
    CAST_PERM_BK = (0x01 << 2),
    CAST_PERM_BQ = (0x01 << 3),

    CAST_PERM_WHITE_MASK = (CAST_PERM_WK | CAST_PERM_WQ),
    CAST_PERM_BLACK_MASK = (CAST_PERM_BK | CAST_PERM_BQ),

};

static void set_perm_state(struct cast_perm_container *cp_cont, const enum cast_perm_bitmap perm);
static void clear_perm_state(struct cast_perm_container *cp_cont, const enum cast_perm_bitmap perm);

struct cast_perm_container cast_perm_init(void) {
    struct cast_perm_container p = {0};
    return p;
}

/**
 * @brief Tests if there are any castle permissions active
 * 
 * @param cp_cont The castle permission struct
 * @return true If there are castle permissions active
 * @return false If there are no castle permissions active
 */
bool cast_perm_has_permissions(const struct cast_perm_container cp_cont) {
    assert(validate_castle_permission_struct(cp_cont));

    return cp_cont.val != CASTLE_PERM_NONE;
}

/**
 * @brief Tests if there is White King-side permissions active
 * 
 * @param cp_cont The castle permission struct
 * @return true If there are white king-side castle permissions active
 * @return false If there are no white king-side castle permissions active
 */
bool cast_perm_has_white_kingside_permissions(const struct cast_perm_container cp_cont) {
    assert(validate_castle_permission_struct(cp_cont));

    return (cp_cont.val & CAST_PERM_WK) != 0;
}

/**
 * @brief Tests if there is White Queen-side permissions active
 * 
 * @param cp_cont The castle permission struct
 * @return true If there are white Queen-side castle permissions active
 * @return false If there are no white Queen-side castle permissions active
 */
bool cast_perm_has_white_queenside_permissions(const struct cast_perm_container cp_cont) {
    assert(validate_castle_permission_struct(cp_cont));

    return (cp_cont.val & CAST_PERM_WQ) != 0;
}

/**
 * @brief      Returns true if there are WHITE castle permissions available
 *
 * @param[in]  cp_cont  The cp container
 *
 * @return     true when white has castle permissions
 */
bool cast_perm_has_white_permissions(const struct cast_perm_container cp_cont) {
    assert(validate_castle_permission_struct(cp_cont));

    return (cp_cont.val & CAST_PERM_WHITE_MASK) != 0;
}

/**
 * @brief Tests if there is Black King-side permissions active
 * 
 * @param cp_cont The castle permission struct
 * @return true If there are black king-side castle permissions active
 * @return false If there are no black king-side castle permissions active
 */
bool cast_perm_has_black_kingside_permissions(const struct cast_perm_container cp_cont) {
    assert(validate_castle_permission_struct(cp_cont));

    return (cp_cont.val & CAST_PERM_BK) != 0;
}

/**
 * @brief Tests if there is Black Queen-side permissions active
 * 
 * @param cp_cont The castle permission struct
 * @return true If there are black Queen-side castle permissions active
 * @return false If there are no black Queen-side castle permissions active
 */
bool cast_perm_has_black_queenside_permissions(const struct cast_perm_container cp_cont) {
    assert(validate_castle_permission_struct(cp_cont));

    return (cp_cont.val & CAST_PERM_BQ) != 0;
}

/**
 * @brief      Returns true if there are BLACK castle permissions available
 *
 * @param[in]  cp_cont  The cp container
 *
 * @return     true when black has castle permissions
 */
bool cast_perm_has_black_permissions(const struct cast_perm_container cp_cont) {
    assert(validate_castle_permission_struct(cp_cont));

    return (cp_cont.val & CAST_PERM_BLACK_MASK) != 0;
}
/**
 * @brief Clears White castle permissions
 * 
 * @param cp_cont The permission container
 */
void cast_perm_clear_white_permissions(struct cast_perm_container *cp_cont) {
    assert(validate_castle_permission_struct(*cp_cont));

    clear_perm_state(cp_cont, CAST_PERM_WK);
    clear_perm_state(cp_cont, CAST_PERM_WQ);
}
/**
 * @brief Clears black castle permissions
 * 
 * @param cp_cont The castle permission container 
 */
void cast_perm_clear_black_permissions(struct cast_perm_container *cp_cont) {
    assert(validate_castle_permission_struct(*cp_cont));

    clear_perm_state(cp_cont, CAST_PERM_BK);
    clear_perm_state(cp_cont, CAST_PERM_BQ);
}

/**
 * @brief               Set the given Castle Permission to the given state
 * @param cp            The Castle Permission to set
 * @param cp_cont       The Castle permission container
 */
void cast_perm_set_permission(const enum castle_permission cp, struct cast_perm_container *cp_cont, const bool state) {
    if (state) {
        switch (cp) {
        case CASTLE_PERM_NONE:
            cp_cont->val = 0;
            break;
        case CASTLE_PERM_WQ:
            set_perm_state(cp_cont, CAST_PERM_WQ);
            break;
        case CASTLE_PERM_WK:
            set_perm_state(cp_cont, CAST_PERM_WK);
            break;
        case CASTLE_PERM_BQ:
            set_perm_state(cp_cont, CAST_PERM_BQ);
            break;
        case CASTLE_PERM_BK:
            set_perm_state(cp_cont, CAST_PERM_BK);
            break;
        default:
            REQUIRE(false, "Invalid castle permissions");
        }
    } else {
        switch (cp) {
        case CASTLE_PERM_NONE:
            cp_cont->val = 0;
            break;
        case CASTLE_PERM_WQ:
            clear_perm_state(cp_cont, CAST_PERM_WQ);
            break;
        case CASTLE_PERM_WK:
            clear_perm_state(cp_cont, CAST_PERM_WK);
            break;
        case CASTLE_PERM_BQ:
            clear_perm_state(cp_cont, CAST_PERM_BQ);
            break;
        case CASTLE_PERM_BK:
            clear_perm_state(cp_cont, CAST_PERM_BK);
            break;
        default:
            REQUIRE(false, "Invalid castle permissions");
        }
    }
}

bool cast_compare_perms(const struct cast_perm_container cp1, const struct cast_perm_container cp2) {
    return cp1.val == cp2.val;
}

bool validate_castle_permission_struct(const struct cast_perm_container cp) {
    uint8_t val = cp.val;
    val = val >> 4; // everything above this it must be zero
    return val == 0;
}

bool validate_castle_permission(const enum castle_permission cp) {
    switch (cp) {
    case CASTLE_PERM_NONE:
    case CASTLE_PERM_WK:
    case CASTLE_PERM_WQ:
    case CASTLE_PERM_BK:
    case CASTLE_PERM_BQ:
        return true;
    default:
        return false;
    }
}

static void set_perm_state(struct cast_perm_container *cp, const enum cast_perm_bitmap perm) {
    cp->val |= perm;
}

static void clear_perm_state(struct cast_perm_container *cp, const enum cast_perm_bitmap perm) {
    cp->val &= ~perm;
}

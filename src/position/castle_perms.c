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
    CAST_PERM_WK = 0x01 << 0,
    CAST_PERM_WQ = 0x01 << 1,
    CAST_PERM_BK = 0x01 << 2,
    CAST_PERM_BQ = 0x01 << 3,
};

enum cast_perm_array_offsets {
    OFFSET_CAST_PERM_NONE = 0,
    OFFSET_CAST_PERM_WK = 1,
    OFFSET_CAST_PERM_WQ = 2,
    OFFSET_CAST_PERM_BK = 3,
    OFFSET_CAST_PERM_BQ = 4,
};

static void set_perm_state(struct cast_perm_container *cp_cont, const enum cast_perm_bitmap perm, const bool state);
static void set_perm(struct cast_perm_container *cp_cont, const enum cast_perm_bitmap perm);
static void clear_perm(struct cast_perm_container *cp_cont, const enum cast_perm_bitmap perm);

struct cast_perm_container cast_perm_init(void) {
    struct cast_perm_container p = {0};
    return p;
}

/**
 * @brief               Tests if a given Castle permission is available
 * @param cp            The Castle Permission to test for
 * @param cp_cont       The Castle permission container
 * @return              True if castle permission is available, false otherwise
 */
bool cast_perm_has_permission(const enum castle_permission cp, const struct cast_perm_container cp_cont) {
    switch (cp) {
    case CASTLE_PERM_NONE:
        return cp_cont.val == 0;
    case CASTLE_PERM_WQ:
        return (cp_cont.val & CAST_PERM_WQ) != 0;
    case CASTLE_PERM_WK:
        return (cp_cont.val & CAST_PERM_WK) != 0;
    case CASTLE_PERM_BQ:
        return (cp_cont.val & CAST_PERM_BQ) != 0;
    case CASTLE_PERM_BK:
        return (cp_cont.val & CAST_PERM_BK) != 0;
    default:
        REQUIRE(false, "Invalid castle permissions");
    }
}

void cast_perm_clear_white_permissions(struct cast_perm_container *cp_cont) {
    set_perm_state(cp_cont, CAST_PERM_WK, false);
    set_perm_state(cp_cont, CAST_PERM_WQ, false);
}
void cast_perm_clear_black_permissions(struct cast_perm_container *cp_cont) {
    set_perm_state(cp_cont, CAST_PERM_BK, false);
    set_perm_state(cp_cont, CAST_PERM_BQ, false);
}

/**
 * @brief               Set the given Castle Permission to the given state
 * @param cp            The Castle Permission to set
 * @param cp_cont       The Castle permission container
 */
void cast_perm_set_permission(const enum castle_permission cp, struct cast_perm_container *cp_cont, const bool state) {
    switch (cp) {
    case CASTLE_PERM_NONE:
        cp_cont->val = 0;
        break;
    case CASTLE_PERM_WQ:
        set_perm_state(cp_cont, CAST_PERM_WQ, state);
        break;
    case CASTLE_PERM_WK:
        set_perm_state(cp_cont, CAST_PERM_WK, state);
        break;
    case CASTLE_PERM_BQ:
        set_perm_state(cp_cont, CAST_PERM_BQ, state);
        break;
    case CASTLE_PERM_BK:
        set_perm_state(cp_cont, CAST_PERM_BK, state);
        break;
    default:
        REQUIRE(false, "Invalid castle permissions");
    }
}

/**
 * @brief               Converts the Castle Permission to an array offset
 * @param               The Castle permission to convert
 * @return              Array offset
 */
uint8_t cast_perm_get_offset(const enum castle_permission cp) {
    switch (cp) {
    case CASTLE_PERM_NONE:
        return OFFSET_CAST_PERM_NONE;
    case CASTLE_PERM_WQ:
        return OFFSET_CAST_PERM_WQ;
    case CASTLE_PERM_WK:
        return OFFSET_CAST_PERM_WK;
    case CASTLE_PERM_BQ:
        return OFFSET_CAST_PERM_BQ;
    case CASTLE_PERM_BK:
        return OFFSET_CAST_PERM_BK;
    default:
        REQUIRE(false, "Invalid castle permissions");
    }
}

bool cast_compare_perms(const struct cast_perm_container cp1, const struct cast_perm_container cp2) {
    return cp1.val == cp2.val;
}

bool validate_castle_permissions(const struct cast_perm_container cp) {
    uint8_t val = cp.val;
    val = val >> 3; // everything above this it must be zero
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

static void set_perm_state(struct cast_perm_container *cp, const enum cast_perm_bitmap perm, const bool state) {
    if (state) {
        set_perm(cp, perm);
    } else {
        clear_perm(cp, perm);
    }
}

static void set_perm(struct cast_perm_container *cp, const enum cast_perm_bitmap perm) {
    cp->val |= perm;
}

static void clear_perm(struct cast_perm_container *cp, const enum cast_perm_bitmap perm) {
    cp->val &= ~perm;
}

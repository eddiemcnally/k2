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

#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "test_castle_permissions.h"
#include "castle_perms.h"
#include <cmocka.h>

void test_castle_permissions_get_set(void **state) {
    struct cast_perm_container cp = {.val = 0};

    cast_perm_set_permission(CASTLE_PERM_WK, &cp, true);
    assert_true(cast_perm_has_white_kingside_permissions(cp));
    cast_perm_set_permission(CASTLE_PERM_WK, &cp, false);
    assert_false(cast_perm_has_white_kingside_permissions(cp));

    cast_perm_set_permission(CASTLE_PERM_WQ, &cp, true);
    assert_true(cast_perm_has_white_queenside_permissions(cp));
    cast_perm_set_permission(CASTLE_PERM_WQ, &cp, false);
    assert_false(cast_perm_has_white_queenside_permissions(cp));

    cast_perm_set_permission(CASTLE_PERM_BK, &cp, true);
    assert_true(cast_perm_has_black_kingside_permissions(cp));
    cast_perm_set_permission(CASTLE_PERM_BK, &cp, false);
    assert_false(cast_perm_has_black_kingside_permissions(cp));

    cast_perm_set_permission(CASTLE_PERM_BQ, &cp, true);
    assert_true(cast_perm_has_black_queenside_permissions(cp));
    cast_perm_set_permission(CASTLE_PERM_BQ, &cp, false);
    assert_false(cast_perm_has_black_queenside_permissions(cp));
}

void test_castle_permissions_no_perms_get_set(void **state) {

    struct cast_perm_container cp = {.val = 0};

    cast_perm_set_permission(CASTLE_PERM_NONE, &cp, true);
    assert_false(cast_perm_has_white_kingside_permissions(cp));
    assert_false(cast_perm_has_white_queenside_permissions(cp));
    assert_false(cast_perm_has_black_kingside_permissions(cp));
    assert_false(cast_perm_has_black_queenside_permissions(cp));

    assert_false(cast_perm_has_permissions(cp));
}

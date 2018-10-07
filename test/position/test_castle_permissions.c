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


#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "castle_perms.h"
#include "test_castle_permissions.h"

void test_castle_permissions_get_set ( void **state )
{
        struct cast_perm cp = {.val = 0};

        cast_perm_set_WK ( &cp, true );
        assert_true ( cast_perm_has_WK ( cp ) );
        cast_perm_set_WK ( &cp, false );
        assert_false ( cast_perm_has_WK ( cp ) );

        cast_perm_set_WQ ( &cp, true );
        assert_true ( cast_perm_has_WQ ( cp ) );
        cast_perm_set_WQ ( &cp, false );
        assert_false ( cast_perm_has_WQ ( cp ) );

        cast_perm_set_BK ( &cp, true );
        assert_true ( cast_perm_has_BK ( cp ) );
        cast_perm_set_BK ( &cp, false );
        assert_false ( cast_perm_has_BK ( cp ) );

        cast_perm_set_BQ ( &cp, true );
        assert_true ( cast_perm_has_BQ ( cp ) );
        cast_perm_set_BQ ( &cp, false );
        assert_false ( cast_perm_has_BQ ( cp ) );
}

void test_castle_permissions_no_perms_get_set ( void **state )
{

        struct cast_perm cp = {.val = 0};

        cast_perm_set_no_perms ( &cp );
        assert_false ( cast_perm_has_WK ( cp ) );
        assert_false ( cast_perm_has_WQ ( cp ) );
        assert_false ( cast_perm_has_BK ( cp ) );
        assert_false ( cast_perm_has_BQ ( cp ) );

        assert_false ( cast_perm_has_perms ( cp ) );
}




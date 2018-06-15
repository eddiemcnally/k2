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
#include "castle_perms.h"

enum castle_perm {
        CAST_PERM_NONE  = 0,
        CAST_PERM_WK    = 0x01 << 0,
        CAST_PERM_WQ    = 0x01 << 1,
        CAST_PERM_BK    = 0x01 << 2,
        CAST_PERM_BQ    = 0x01 << 3,
};

static void set_perm_state ( uint8_t* cp, const enum castle_perm perm, const bool state );
static void set_perm ( uint8_t* cp, const enum castle_perm perm );
static void clear_perm ( uint8_t* cp, const enum castle_perm perm );


bool cast_perm_has_WK ( const uint8_t cp )
{
        return ( cp & CAST_PERM_WK ) != 0;
}
bool cast_perm_has_WQ ( const uint8_t cp )
{
        return ( cp & CAST_PERM_WQ ) != 0;

}
bool cast_perm_has_BK ( const uint8_t cp )
{
        return ( cp & CAST_PERM_BK ) != 0;
}
bool cast_perm_has_BQ ( const uint8_t cp )
{
        return ( cp & CAST_PERM_BQ ) != 0;
}
bool cast_perm_has_perms ( const uint8_t cp )
{
        return cp != 0;
}


void cast_perm_set_WK ( uint8_t* cp, const bool enabled )
{
        set_perm_state ( cp, CAST_PERM_WK, enabled );

}
void cast_perm_set_WQ ( uint8_t* cp, const bool enabled )
{
        set_perm_state ( cp, CAST_PERM_WQ, enabled );

}
void cast_perm_set_BK ( uint8_t* cp, const bool enabled )
{
        set_perm_state ( cp, CAST_PERM_BK, enabled );

}
void cast_perm_set_BQ ( uint8_t* cp, const bool enabled )
{
        set_perm_state ( cp, CAST_PERM_BQ, enabled );

}
void cast_perm_set_no_perms ( uint8_t* cp )
{
        *cp = 0;
}

static void set_perm_state ( uint8_t* cp, const enum castle_perm perm, const bool state )
{
        if ( state ) {
                set_perm ( cp, perm );
        } else {
                clear_perm ( cp, perm );
        }
}



static void set_perm ( uint8_t* cp, const enum castle_perm perm )
{
        *cp |= perm;
}

static void clear_perm ( uint8_t* cp, const enum castle_perm perm )
{
        *cp &= ~perm;
}

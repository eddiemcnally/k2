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

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct cast_perm_container {
    uint8_t val;
};

enum castle_permission {
    CP_NONE = 0,
    CP_WQ,
    CP_WK,
    CP_BQ,
    CP_BK,
};

// include the "no-permisisons" state
#define NUM_CASTLE_PERMS 5

struct cast_perm_container cast_perm_init(void);

bool cast_perm_has_permission(const enum castle_permission cp,
                              const struct cast_perm_container cp_cont);
void cast_perm_set_permission(const enum castle_permission cp,
                              struct cast_perm_container *cp_cont,
                              const bool state);
bool cast_compare_perms(const struct cast_perm_container cp1,
                        const struct cast_perm_container cp2);
uint8_t cast_perm_get_offset(const enum castle_permission cp);

void cast_perm_clear_white_permissions(struct cast_perm_container *cp_cont);
void cast_perm_clear_black_permissions(struct cast_perm_container *cp_cont);

bool validate_castle_permissions(const struct cast_perm_container cp);
bool validate_castle_permission(const enum castle_permission cp);

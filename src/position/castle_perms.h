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

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct cast_perm {
    uint8_t val;
};

bool cast_perm_has_WK(const struct cast_perm cp);
bool cast_perm_has_WQ(const struct cast_perm cp);
bool cast_perm_has_BK(const struct cast_perm cp);
bool cast_perm_has_BQ(const struct cast_perm cp);
bool cast_perm_has_perms(const struct cast_perm cp);

void cast_perm_set_WK(struct cast_perm* cp, const bool enabled);
void cast_perm_set_WQ(struct cast_perm* cp, const bool enabled);
void cast_perm_set_BK(struct cast_perm* cp, const bool enabled);
void cast_perm_set_BQ(struct cast_perm* cp, const bool enabled);
void cast_perm_set_no_perms(struct cast_perm* cp);

bool cast_compare_perms(const struct cast_perm cp1, const struct cast_perm cp2);

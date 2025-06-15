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

/*! @addtogroup Utils
 *
 * @ingroup Utils
 * @{
 * @details General utility functions for the engine
 *
 */

#include "utils.h"
#include <ctype.h>
#include <execinfo.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>

/**
 * @brief       Prints the current stack to STD_OUT
 */
void __attribute__((noinline)) print_stacktrace(void) {
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = (size_t)backtrace(array, 10);
    strings = backtrace_symbols(array, (int)size);

    printf("Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++) {
        printf("%s\n", strings[i]);
    }

    free(strings);
}

void __attribute__((noinline)) print_stacktrace_and_exit(const char *file, const int line, const char *function,
                                                         const char *text) {
    printf("*** Error ***\n");
    printf("\tFile     :    %s\n", file);
    printf("\tLine     :    %d\n", line);
    printf("\tFunction :    %s\n", function);
    printf("\tText     :    %s\n", text);
    print_stacktrace();
    exit(-1);
}

/**
 * @brief       Returns the current time of day in milliseconds
 * @return      Time of day in millis
 */
double get_time_of_day_in_secs(void) {
    struct timeval tp;

    int errno = gettimeofday(&tp, NULL);

    if (errno == 0) {
        double time_in_secs = (double)tp.tv_sec + (double)tp.tv_usec / 1000000;
        return time_in_secs;
    }
    return -1;
}

/**
 * @brief       Returns elapsed time between the given time and now, in milliseconds.
 * @return      Elapsed time in milliseconds
 */
double get_elapsed_time_in_secs(double start_time) {
    double now_in_secs = get_time_of_day_in_secs();
    return (now_in_secs - start_time);
}

/**
 * @brief Rounds a number down to the nearest power of 2
 * 
 * @param n the number
 * @return uint64_t the rounded down number
 */
uint64_t round_down_to_nearest_power_2(uint64_t n) {
    uint64_t res = 0;
    for (uint64_t i = n; i >= 1; i--) {
        // If i is a power of 2
        if ((i & (i - 1)) == 0) {
            res = i;
            break;
        }
    }
    return res;
}

/**
 * @brief Issues a cache pre-fetch instruction for the given address
 * 
 * @param addr The address to prefetch.
 */
void prefetch(void *addr) {
    __builtin_prefetch(addr);
}

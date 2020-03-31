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

#define _GNU_SOURCE
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
 * @brief       Sets CPU affinity to CPU 1, and sets pririty to max
 */
void set_priority_and_affinity(void) {
    // set up CPU affinity
    cpu_set_t my_set;
    CPU_ZERO(&my_set);
    CPU_SET(1, &my_set);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &my_set) > 0) {
        printf("affinity error");
        exit(-1);
    }
    // set process priority to max
    if (setpriority(PRIO_PROCESS, 0, PRIO_MAX) != 0) {
        printf("process priority error");
        exit(-1);
    }
}

/**
 * @brief       Prints the current stack to STD_OUT
 */
void print_stacktrace(void) {
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

    return 0;
}

/**
 * @brief       Returns elapsed time between the given time and now, in milliseconds.
 * @return      Elapsed time in milliseconds
 */
double get_elapsed_time_in_secs(double start_time) {
    double now_in_secs = get_time_of_day_in_secs();
    return (now_in_secs - start_time);
}

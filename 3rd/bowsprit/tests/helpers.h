/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef TESTS_HELPERS_H
#define TESTS_HELPERS_H

#include <stdlib.h>

#include <libcork/core.h>


/*-----------------------------------------------------------------------
 * Allocators
 */

/* Use a custom allocator that debugs every allocation. */

static void
setup_allocator(void)
{
    struct cork_alloc  *debug = cork_debug_alloc_new(cork_allocator);
    cork_set_allocator(debug);
}


/*-----------------------------------------------------------------------
 * Error reporting
 */

#if !defined(PRINT_EXPECTED_FAILURES)
#define PRINT_EXPECTED_FAILURES  1
#endif

#if PRINT_EXPECTED_FAILURES
#define print_expected_failure() \
    printf("[expected: %s]\n", cork_error_message());
#else
#define print_expected_failure()  /* do nothing */
#endif


#define DESCRIBE_TEST \
    fprintf(stderr, "--- %s\n", __func__);


#define fail_if_error(call) \
    do { \
        call; \
        if (cork_error_occurred()) { \
            fail("%s", cork_error_message()); \
        } \
    } while (0)

#define fail_unless_error(call, ...) \
    do { \
        call; \
        if (!cork_error_occurred()) { \
            fail(__VA_ARGS__); \
        } else { \
            print_expected_failure(); \
        } \
        cork_error_clear(); \
    } while (0)

#define fail_unless_equal(what, format, type, expected, actual) \
    do { \
        type  __expected = (expected); \
        type  __actual = (actual); \
        fail_unless(__expected == __actual, \
                    "%s not equal (expected " format \
                    ", got " format ")", \
                    (what), __expected, __actual); \
    } while (0)

#define fail_unless_streq(what, expected, actual) \
    (fail_unless(strcmp((expected), (actual)) == 0, \
                 "%s not equal (expected \"%s\", got \"%s\")", \
                 (what), (expected), (actual)))


static void
initialize_tests(void)
{
    setup_allocator();
}

static void
finalize_tests(void)
{
}


#endif /* TESTS_HELPERS_H */

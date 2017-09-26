/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>
#include <libcork/core.h>
#include <libcork/ds.h>

#include "bowsprit.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Periodic
 */

static int
fire(void *user_data, struct bws_snapshot *curr, struct bws_snapshot *prev,
     cork_timestamp now)
{
    size_t  *count = user_data;
    (*count)++;
    return 0;
}

START_TEST(test_periodic_fire)
{
    struct bws_ctx  *ctx;
    struct bws_periodic  *periodic;
    size_t  fired_count = 0;
    cork_timestamp  seconds;
    cork_timestamp  now;

    DESCRIBE_TEST;

    /* The particular time doesn't matter here since we're mocking the
     * timestamps. */
    cork_timestamp_init_sec(&now, 1388534400 /* 2014-01-01 00:00:00 UTC */);
    cork_timestamp_init_sec(&seconds, 1);

    ctx = bws_ctx_new("localhost");
    periodic = bws_periodic_new(ctx, &fired_count, NULL, fire);

    fail_unless_equal("Fire count", "%zu", size_t, 0, fired_count);

    fail_if_error(bws_periodic_mocked_poll(periodic, now));
    fail_unless_equal("Fire count", "%zu", size_t, 1, fired_count);

    /* Default interval is 30 seconds, so we shouldn't get another fire until we
     * cross that threshold. */
    now += 5 * seconds;
    fail_if_error(bws_periodic_mocked_poll(periodic, now));
    fail_unless_equal("Fire count", "%zu", size_t, 1, fired_count);

    now += 15 * seconds;
    fail_if_error(bws_periodic_mocked_poll(periodic, now));
    fail_unless_equal("Fire count", "%zu", size_t, 1, fired_count);

    now += 10 * seconds;
    fail_if_error(bws_periodic_mocked_poll(periodic, now));
    fail_unless_equal("Fire count", "%zu", size_t, 2, fired_count);

    bws_periodic_free(periodic);
    bws_ctx_free(ctx);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("periodic");

    TCase  *tc_periodic = tcase_create("periodic");
    tcase_add_test(tc_periodic, test_periodic_fire);
    suite_add_tcase(s, tc_periodic);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    initialize_tests();
    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    finalize_tests();

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}

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
 * Statistics
 */

static void
check_render(struct bws_ctx *ctx, const char *expected)
{
    struct cork_buffer  buf = CORK_BUFFER_INIT();
    struct bws_snapshot  *snapshot = bws_snapshot_new();
    bws_ctx_snapshot(ctx, snapshot);
    bws_snapshot_sort(snapshot);
    bws_snapshot_render_to_buffer(snapshot, &buf);
    fail_unless_streq("Rendering", expected, (char *) buf.buf);
    bws_snapshot_free(snapshot);
    cork_buffer_done(&buf);
}

START_TEST(test_nothing)
{
    struct bws_ctx  *ctx;
    struct bws_plugin  *plugin;

    DESCRIBE_TEST;

    ctx = bws_ctx_new("localhost");
    plugin = bws_plugin_new(ctx, "animals", NULL);
    bws_gauge_new(plugin, "ribbits", "frog");
    bws_gauge_new(plugin, "meows", "cat");
    bws_gauge_new(plugin, "barks", "dog");

    check_render
        (ctx,
         "0 animals/barks-dog\n"
         "0 animals/meows-cat\n"
         "0 animals/ribbits-frog\n"
         );

    bws_ctx_free(ctx);
}
END_TEST

START_TEST(test_derive_01)
{
    struct bws_ctx  *ctx;
    struct bws_plugin  *plugin;
    struct bws_derive  *frog;
    struct bws_derive  *cat;
    struct bws_derive  *dog;

    DESCRIBE_TEST;

    ctx = bws_ctx_new("localhost");
    plugin = bws_plugin_new(ctx, "animals", NULL);
    frog = bws_derive_new(plugin, "ribbits", "frog");
    cat = bws_derive_new(plugin, "meows", "cat");
    dog = bws_derive_new(plugin, "barks", "dog");

    bws_derive_set(frog, 1);
    fail_unless_equal("cat",  "%" PRIu64, uint64_t, 10, bws_derive_add(cat, 10));
    fail_unless_equal("frog", "%" PRIu64, uint64_t,  2, bws_derive_inc(frog));
    fail_unless_equal("frog", "%" PRIu64, uint64_t,  3, bws_derive_inc(frog));
    fail_unless_equal("dog",  "%" PRIu64, uint64_t, 10, bws_derive_add(dog, 10));

    check_render
        (ctx,
         "10 animals/barks-dog\n"
         "10 animals/meows-cat\n"
         " 3 animals/ribbits-frog\n"
         );

    fail_unless_equal("dog",  "%" PRIu64, uint64_t, 15, bws_derive_add(dog, 5));
    fail_unless_equal("frog", "%" PRIu64, uint64_t,  4, bws_derive_inc(frog));

    check_render
        (ctx,
         "15 animals/barks-dog\n"
         "10 animals/meows-cat\n"
         " 4 animals/ribbits-frog\n"
         );

    bws_ctx_free(ctx);
}
END_TEST

START_TEST(test_gauge_01)
{
    struct bws_ctx  *ctx;
    struct bws_plugin  *plugin;
    struct bws_gauge  *frog;
    struct bws_gauge  *cat;
    struct bws_gauge  *dog;

    DESCRIBE_TEST;

    ctx = bws_ctx_new("localhost");
    plugin = bws_plugin_new(ctx, "animals", NULL);
    frog = bws_gauge_new(plugin, "ribbits", "frog");
    cat = bws_gauge_new(plugin, "meows", "cat");
    dog = bws_gauge_new(plugin, "barks", "dog");

    bws_gauge_set(frog, 1);
    fail_unless_equal("cat",  "%" PRIu64, uint64_t, 10, bws_gauge_add(cat, 10));
    fail_unless_equal("frog", "%" PRIu64, uint64_t,  2, bws_gauge_inc(frog));
    fail_unless_equal("frog", "%" PRIu64, uint64_t,  3, bws_gauge_inc(frog));
    fail_unless_equal("dog",  "%" PRIu64, uint64_t, 10, bws_gauge_add(dog, 10));

    check_render
        (ctx,
         "10 animals/barks-dog\n"
         "10 animals/meows-cat\n"
         " 3 animals/ribbits-frog\n"
         );

    fail_unless_equal("frog", "%" PRIu64, uint64_t,  4, bws_gauge_inc(frog));
    fail_unless_equal("cat",  "%" PRIu64, uint64_t,  5, bws_gauge_sub(cat, 5));

    check_render
        (ctx,
         "10 animals/barks-dog\n"
         " 5 animals/meows-cat\n"
         " 4 animals/ribbits-frog\n"
         );

    fail_unless_equal("frog", "%" PRIu64, uint64_t,  3, bws_gauge_dec(frog));
    fail_unless_equal("frog", "%" PRIu64, uint64_t,  2, bws_gauge_dec(frog));
    fail_unless_equal("dog",  "%" PRIu64, uint64_t,  0, bws_gauge_sub(dog, 10));

    check_render
        (ctx,
         "5 animals/meows-cat\n"
         "2 animals/ribbits-frog\n"
         "0 animals/barks-dog\n"
         );

    bws_ctx_free(ctx);
}
END_TEST

START_TEST(test_multi_01)
{
    struct bws_ctx  *ctx;
    struct bws_plugin  *plugin;
    struct bws_measurement  *if_packets;
    struct bws_derive  *tx;
    struct bws_derive  *rx;

    DESCRIBE_TEST;

    ctx = bws_ctx_new("localhost");
    plugin = bws_plugin_new(ctx, "network", NULL);
    if_packets = bws_measurement_new(plugin, "if_packets", "eth0");
    tx = bws_measurement_add_derive(if_packets, "tx");
    rx = bws_measurement_add_derive(if_packets, "rx");

    fail_unless_equal("tx", "%" PRIu64, uint64_t, 10, bws_derive_add(tx, 10));
    fail_unless_equal("rx", "%" PRIu64, uint64_t, 20, bws_derive_add(rx, 20));

    check_render
        (ctx,
         "20 network/if_packets-eth0/rx\n"
         "10 network/if_packets-eth0/tx\n"
         );

    bws_ctx_free(ctx);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("stats");

    TCase  *tc_stats = tcase_create("stats");
    tcase_add_test(tc_stats, test_nothing);
    tcase_add_test(tc_stats, test_derive_01);
    tcase_add_test(tc_stats, test_gauge_01);
    tcase_add_test(tc_stats, test_multi_01);
    suite_add_tcase(s, tc_stats);

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

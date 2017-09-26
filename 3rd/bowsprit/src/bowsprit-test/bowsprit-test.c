/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <clogger.h>
#include <libcork/core.h>
#include <libcork/threads.h>

#include "bowsprit.h"


#define ri_check_exit(call) \
    do { \
        int  __rc = (call); \
        if (CORK_UNLIKELY(__rc != 0)) { \
            fprintf(stderr, "%s\n", cork_error_message()); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)


/* TODO: Eventually we might want to make the various knobs configurable, and
 * let the caller control what statistics to make and when to increment them.
 * That way we can have a wider variety of test cases.  But this works fine for
 * now. */

int
main(int argc, const char **argv)
{
    struct bws_ctx  *ctx;
    struct bws_plugin  *plugin;
    struct bws_gauge  *frog;
    struct bws_derive  *cat;
    struct bws_gauge  *dog;
    struct bws_rotated_files  *rotated;
    struct bws_rotated_files  *slugged_rotated;
    struct bws_periodic  *periodic;
    struct bws_periodic  *slugged_periodic;
    struct bws_periodic  *logger;
    cork_timestamp  seconds;
    cork_timestamp  now;

    clog_setup_logging();

    /* The particular time doesn't matter here since we're mocking the
     * timestamps. */
    cork_timestamp_init_sec(&now, 1388534400 /* 2014-01-01 00:00:00 UTC */);
    cork_timestamp_init_sec(&seconds, 1);

    /* Set up our measurements. */
    ctx = bws_ctx_new("localhost");
    plugin = bws_plugin_new(ctx, "test", "1");
    frog = bws_gauge_new(plugin, "frog", NULL);
    cat = bws_derive_new(plugin, "cat", NULL);
    dog = bws_gauge_new(plugin, "dog", NULL);

    /* Create a file rotator. */
    rotated = bws_rotated_files_new(ctx, ".");
    periodic = bws_rotated_files_periodic(rotated);
    bws_rotated_files_set_file_duration(rotated, 1);

    /* Create a file rotator with a slug. */
    slugged_rotated = bws_rotated_files_new(ctx, ".");
    slugged_periodic = bws_rotated_files_periodic(slugged_rotated);
    bws_rotated_files_set_file_duration(slugged_rotated, 1);
    bws_rotated_files_set_filename_slug(slugged_rotated, "slugged");

    /* Create the logger. */
    logger = bws_logger_new(ctx, "bowsprit-test", CLOG_LEVEL_INFO);

    /* Prime the pump with an initial timestamp. */
    ri_check_exit(bws_periodic_mocked_poll(periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(slugged_periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(logger, now));

    /* Start incrementing some stats and writing to files. */
    bws_gauge_inc(frog);
    bws_derive_inc(cat);
    bws_gauge_inc(dog);

    /* Default interval is 30 seconds, so we shouldn't have any output yet. */
    now += 5 * seconds;
    ri_check_exit(bws_periodic_mocked_poll(periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(slugged_periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(logger, now));

    /* After 30 seconds we get our first output. */
    bws_derive_inc(cat);
    now += 25 * seconds;
    ri_check_exit(bws_periodic_mocked_poll(periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(slugged_periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(logger, now));
    /* Expected output: [file 1] 1, 2, 1 */

    /* No more output until after 60 seconds. */
    bws_gauge_inc(frog);
    bws_derive_inc(cat);
    now += 15 * seconds;
    ri_check_exit(bws_periodic_mocked_poll(periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(slugged_periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(logger, now));

    /* After 60 seconds we get another output, and roll over to a new file. */
    bws_derive_inc(cat);
    bws_gauge_inc(dog);
    now += 15 * seconds;
    ri_check_exit(bws_periodic_mocked_poll(periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(slugged_periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(logger, now));
    /* Expected output: [file 2] 2, 4, 2 */

    /* No more output until after 90 seconds. */
    bws_gauge_inc(frog);
    bws_gauge_inc(dog);
    now += 15 * seconds;
    ri_check_exit(bws_periodic_mocked_poll(periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(slugged_periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(logger, now));

    /* After 90 seconds we get another output, in the same file. */
    bws_derive_inc(cat);
    bws_gauge_inc(dog);
    now += 15 * seconds;
    ri_check_exit(bws_periodic_mocked_poll(periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(slugged_periodic, now));
    ri_check_exit(bws_periodic_mocked_poll(logger, now));
    /* Expected output: [file 2] 3, 5, 4 */

    bws_rotated_files_free(rotated);
    bws_rotated_files_free(slugged_rotated);
    bws_periodic_free(logger);
    bws_ctx_free(ctx);
    return 0;
}

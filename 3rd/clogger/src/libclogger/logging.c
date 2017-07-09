/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include <libcork/core.h>
#include <libcork/os.h>
#include <libcork/helpers/errors.h>

#include "clogger/api.h"
#include "clogger/error.h"
#include "clogger/handlers.h"
#include "clogger/logging.h"


static const char  *default_format = CLOG_DEFAULT_FORMAT;
static struct clog_handler  *stderr_handler = NULL;
static struct clog_handler  *filter_handler = NULL;

void
clog_set_default_format(const char *fmt)
{
    default_format = fmt;
}


static void
clog_teardown_logging(void)
{
    if (filter_handler != NULL) {
        clog_handler_pop_process(filter_handler);
        clog_handler_free(filter_handler);
    }

    clog_handler_pop_process(stderr_handler);
    clog_handler_free(stderr_handler);
}

int
clog_setup_logging(void)
{
    const char  *value;
    const char  *fmt;

#define test_level(what) \
    if (strcasecmp(value, #what) == 0) { \
        clog_set_minimum_level(CLOG_LEVEL_##what); \
    }

    value = cork_env_get(NULL, "CLOG");
    if (value != NULL) {
        test_level(NONE)
        else test_level(CRITICAL)
        else test_level(ERROR)
        else test_level(WARNING)
        else test_level(NOTICE)
        else test_level(INFO)
        else test_level(DEBUG)
        else test_level(TRACE)
        else {
            clog_bad_config("Unknown CLOG level %s", value);
            return -1;
        }
    }

    value = cork_env_get(NULL, "CLOG_FORMAT");
    fmt = (value == NULL)? default_format: value;

    rip_check(stderr_handler = clog_stderr_handler_new(fmt));
    clog_handler_push_process(stderr_handler);

    value = cork_env_get(NULL, "CLOG_CHANNELS");
    if (value != NULL) {
        struct clog_keep_filter  *filter = clog_keep_filter_new();
        clog_keep_filter_add_many(filter, value);
        filter_handler = clog_keep_filter_handler(filter);
        clog_handler_push_process(filter_handler);
    }

    cork_cleanup_at_exit(0, clog_teardown_logging);
    return 0;
}

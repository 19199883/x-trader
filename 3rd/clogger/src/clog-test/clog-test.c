/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>

#include <libcork/core.h>
#include <libcork/helpers/errors.h>

#include "clogger.h"


int
main(int argc, const char **argv)
{
    struct cork_alloc  *debug = cork_debug_alloc_new(cork_allocator);
    cork_set_allocator(debug);

    if (clog_setup_logging() != 0) {
        fprintf(stderr, "%s\n", cork_error_message());
        exit(EXIT_FAILURE);
    }

    /* Produce a whole bunch of log messages */
    clog_channel_critical("main", "This is a critical message");
    clog_channel_error("main", "This is a error message");
    clog_channel_warning("main", "This is a warning message");
    clog_channel_notice("main", "This is a notice message");
    clog_channel_info("main", "This is a info message");
    clog_channel_debug("main", "This is a debug message");
    clog_channel_trace("main", "This is a trace message");

    /* And some more from a different channel */
    clog_channel_critical("libclogger", "This is a critical message");
    clog_channel_error("libclogger", "This is a error message");
    clog_channel_warning("libclogger", "This is a warning message");
    clog_channel_notice("libclogger", "This is a notice message");
    clog_channel_info("libclogger", "This is a info message");
    clog_channel_debug("libclogger", "This is a debug message");
    clog_channel_trace("libclogger", "This is a trace message");

    return 0;
}

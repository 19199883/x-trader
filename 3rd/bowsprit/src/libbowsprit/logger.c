/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>

#include <clogger.h>
#include <libcork/core.h>
#include <libcork/ds.h>

#include "bowsprit.h"


/*-----------------------------------------------------------------------
 * File rotation
 */

#define DEFAULT_FILE_DURATION_MIN  10

struct bws_logger {
    struct bws_periodic  *periodic;
    struct cork_buffer  buf;
    const char  *channel;
    enum clog_level  level;
};

static int
bws_logger__run(void *user_data, struct bws_snapshot *curr,
                struct bws_snapshot *prev, cork_timestamp now)
{
    struct bws_logger  *logger = user_data;
    unsigned int  interval_sec = bws_periodic_interval(logger->periodic);
    int  width = bws_snapshot_max_width(curr);
    size_t  i;

    cork_buffer_clear(&logger->buf);
    cork_timestamp_format_utc(now, "%Y-%m-%d %H:%M:%S", &logger->buf);
    clog_log_channel
        (logger->level, logger->channel,
         "Statistics as of %s", (char *) logger->buf.buf);

    if (prev == NULL) {
        for (i = 0; i < curr->count; i++) {
            struct bws_value_snapshot  *curr_value = &curr->values[i];
            cork_buffer_clear(&logger->buf);
            bws_value_snapshot_render_delta_to_buffer
                (curr_value, NULL, &logger->buf, interval_sec, width);
            clog_log_channel
                (logger->level, logger->channel,
                 "%s", (char *) logger->buf.buf);
        }
    } else {
        for (i = 0; i < curr->count; i++) {
            struct bws_value_snapshot  *curr_value = &curr->values[i];
            struct bws_value_snapshot  *prev_value = &prev->values[i];
            cork_buffer_clear(&logger->buf);
            bws_value_snapshot_render_delta_to_buffer
                (curr_value, prev_value, &logger->buf, interval_sec, width);
            clog_log_channel
                (logger->level, logger->channel,
                 "%s", (char *) logger->buf.buf);
        }
    }

    return 0;
}

static void
bws_logger__free(void *user_data)
{
    struct bws_logger  *logger = user_data;
    cork_strfree(logger->channel);
    cork_buffer_done(&logger->buf);
    cork_delete(struct bws_logger, logger);
}

struct bws_periodic *
bws_logger_new(struct bws_ctx *ctx, const char *channel, enum clog_level level)
{
    struct bws_logger  *logger = cork_new(struct bws_logger);
    cork_buffer_init(&logger->buf);
    logger->channel = cork_strdup(channel);
    logger->level = level;
    logger->periodic = bws_periodic_new
        (ctx, logger, bws_logger__free, bws_logger__run);
    return logger->periodic;
}

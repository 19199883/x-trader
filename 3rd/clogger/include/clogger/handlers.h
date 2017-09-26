/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012-2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef CLOGGER_HANDLERS_H
#define CLOGGER_HANDLERS_H

#include <stdio.h>

#include <libcork/core.h>
#include <libcork/ds.h>


/*-----------------------------------------------------------------------
 * Null handler
 */

struct clog_handler *
clog_null_handler_new(void);


/*-----------------------------------------------------------------------
 * Stream output handler
 */

struct clog_handler *
clog_stderr_handler_new(const char *fmt);


struct clog_handler *
clog_stream_handler_new_fp(FILE *fp, bool should_close, const char *fmt);

struct clog_handler *
clog_stream_handler_new_consumer(struct cork_stream_consumer *consumer,
                                 const char *fmt);


/*-----------------------------------------------------------------------
 * Channel name filter
 */

struct clog_keep_filter *
clog_keep_filter_new(void);

void
clog_keep_filter_free(struct clog_keep_filter *filter);

void
clog_keep_filter_add(struct clog_keep_filter *filter, const char *channel);

void
clog_keep_filter_add_many(struct clog_keep_filter *filter, const char *str);

struct clog_handler *
clog_keep_filter_handler(struct clog_keep_filter *filter);


#endif /* CLOGGER_HANDLERS_H */

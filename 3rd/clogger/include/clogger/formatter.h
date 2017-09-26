/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012-2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef CLOGGER_FORMATTER_H
#define CLOGGER_FORMATTER_H

#include <libcork/core.h>
#include <libcork/ds.h>

#include <clogger/api.h>


struct clog_formatter;

struct clog_formatter *
clog_formatter_new(const char *format);

void
clog_formatter_free(struct clog_formatter *fmt);

int
clog_formatter_start(struct clog_formatter *fmt);

int
clog_formatter_annotation(struct clog_formatter *fmt, const char *key,
                          const char *value);

int
clog_formatter_finish(struct clog_formatter *fmt, struct clog_message *msg,
                      struct cork_buffer *dest);


#endif /* CLOGGER_FORMATTER_H */

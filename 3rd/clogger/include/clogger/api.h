/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012-2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef CLOGGER_API_H
#define CLOGGER_API_H

#include <stdarg.h>

#include <libcork/core.h>


enum clog_level {
    CLOG_LEVEL_NONE = 0,
    CLOG_LEVEL_CRITICAL,
    CLOG_LEVEL_ERROR,
    CLOG_LEVEL_WARNING,
    CLOG_LEVEL_NOTICE,
    CLOG_LEVEL_INFO,
    CLOG_LEVEL_DEBUG,
    CLOG_LEVEL_TRACE
};

CORK_ATTR_PURE
const char *
clog_level_name(enum clog_level level);

CORK_ATTR_PURE
const char *
clog_level_name_fixed_width(enum clog_level level);


struct clog_message {
    enum clog_level  level;
    const char  *channel;
    const char  *format;
    va_list  args;
};


#define CLOG_CONTINUE  0
#define CLOG_FAILURE  -1
#define CLOG_SKIP     -2

struct clog_handler {
    int
    (*annotation)(struct clog_handler *handler, struct clog_message *msg,
                  const char *key, const char *value);

    int
    (*message)(struct clog_handler *handler, struct clog_message *msg);

    void
    (*free)(struct clog_handler *handler);

    struct clog_handler  *next;
};

#define clog_handler_annotation(handler, msg, key, value) \
    ((handler)->annotation((handler), (msg), (key), (value)))

#define clog_handler_message(handler, msg) \
    ((handler)->message((handler), (msg)))

#define clog_handler_free(handler) \
    ((handler)->free((handler)))


void
clog_handler_push_process(struct clog_handler *handler);

int
clog_handler_pop_process(struct clog_handler *handler);

void
clog_handler_push_thread(struct clog_handler *handler);

int
clog_handler_pop_thread(struct clog_handler *handler);


int
clog_process_message(struct clog_message *msg);

int
clog_annotate_message(struct clog_handler *handler, struct clog_message *msg,
                      const char *key, const char *value);

void
_clog_log_channel(enum clog_level level, const char *channel,
                  const char *format, ...)
    CORK_ATTR_PRINTF(3,4);

#define clog_log_channel(level, channel, ...) \
    (CORK_UNLIKELY((level) <= clog_minimum_level)? \
     _clog_log_channel((level), (channel), __VA_ARGS__): (void) 0)

#define clog_log(level, ...) \
    clog_log_channel((level), CLOG_CHANNEL, __VA_ARGS__)

#define clog_critical(...)  clog_log(CLOG_LEVEL_CRITICAL, __VA_ARGS__)
#define clog_error(...)     clog_log(CLOG_LEVEL_ERROR, __VA_ARGS__)
#define clog_warning(...)   clog_log(CLOG_LEVEL_WARNING, __VA_ARGS__)
#define clog_notice(...)    clog_log(CLOG_LEVEL_NOTICE, __VA_ARGS__)
#define clog_info(...)      clog_log(CLOG_LEVEL_INFO, __VA_ARGS__)
#define clog_debug(...)     clog_log(CLOG_LEVEL_DEBUG, __VA_ARGS__)
#define clog_trace(...)     clog_log(CLOG_LEVEL_TRACE, __VA_ARGS__)

#define clog_channel_critical(ch, ...) \
    clog_log_channel(CLOG_LEVEL_CRITICAL, (ch), __VA_ARGS__)
#define clog_channel_error(ch, ...) \
    clog_log_channel(CLOG_LEVEL_ERROR, (ch), __VA_ARGS__)
#define clog_channel_warning(ch, ...) \
    clog_log_channel(CLOG_LEVEL_WARNING, (ch), __VA_ARGS__)
#define clog_channel_notice(ch, ...) \
    clog_log_channel(CLOG_LEVEL_NOTICE, (ch), __VA_ARGS__)
#define clog_channel_info(ch, ...) \
    clog_log_channel(CLOG_LEVEL_INFO, (ch), __VA_ARGS__)
#define clog_channel_debug(ch, ...) \
    clog_log_channel(CLOG_LEVEL_DEBUG, (ch), __VA_ARGS__)
#define clog_channel_trace(ch, ...) \
    clog_log_channel(CLOG_LEVEL_TRACE, (ch), __VA_ARGS__)

extern enum clog_level  clog_minimum_level;

void
clog_set_minimum_level(enum clog_level level);


#endif /* CLOGGER_API_H */

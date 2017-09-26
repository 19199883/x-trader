/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdarg.h>

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/threads.h>
#include <libcork/helpers/errors.h>

#include "clogger/api.h"
#include "clogger/error.h"


enum clog_level  clog_minimum_level = CLOG_LEVEL_WARNING;

static bool can_push_process_handlers = true;
static struct clog_handler  *process_stack = NULL;
cork_tls(struct clog_handler *, thread_stack);


void
clog_handler_push_process(struct clog_handler *handler)
{
    assert(can_push_process_handlers);
    handler->next = process_stack;
    process_stack = handler;
}

int
clog_handler_pop_process(struct clog_handler *handler)
{
    if (CORK_LIKELY(process_stack == handler)) {
        process_stack = handler->next;
        handler->next = NULL;
        return 0;
    } else {
        clog_bad_stack("Unexpected handler when popping from process stack");
        return -1;
    }
}

void
clog_handler_push_thread(struct clog_handler *handler)
{
    struct clog_handler  **thread_stack = thread_stack_get();
    handler->next = *thread_stack;
    *thread_stack = handler;
    can_push_process_handlers = false;
}

int
clog_handler_pop_thread(struct clog_handler *handler)
{
    struct clog_handler  **thread_stack = thread_stack_get();

    if (CORK_LIKELY(*thread_stack == handler)) {
        *thread_stack = handler->next;
        handler->next = NULL;
        return 0;
    } else {
        clog_bad_stack("Unexpected handler when popping from thread stack");
        return -1;
    }
}


int
clog_process_message(struct clog_message *msg)
{
    /* First send the message through the handlers in the thread stack, and
     * then the handlers in the process stack.  If any of them return
     * CLOG_SKIP_MESSAGE, immediately abort the processing of the message. */

    int  rc;
    struct clog_handler  **thread_stack = thread_stack_get();
    struct clog_handler  *handler;

    for (handler = *thread_stack; handler != NULL; handler = handler->next) {
        ei_check(rc = clog_handler_message(handler, msg));
    }

    for (handler = process_stack; handler != NULL; handler = handler->next) {
        ei_check(rc = clog_handler_message(handler, msg));
    }

    return 0;

error:
    if (rc == CLOG_SKIP) {
        return 0;
    } else {
        return rc;
    }
}

int
clog_annotate_message(struct clog_handler *self, struct clog_message *msg,
                      const char *key, const char *value)
{
    /* First send the annotation through the handlers in the thread stack, and
     * then the handlers in the process stack.  If any of them return
     * CLOG_SKIP_MESSAGE, immediately abort the processing of the message. */

    int  rc;
    struct clog_handler  **thread_stack = thread_stack_get();
    struct clog_handler  *handler;
    bool  found_handler = false;

    /* We're not supposed to send the annotation to any handlers "above" the
     * current one in the stack. */

    for (handler = *thread_stack; handler != NULL; handler = handler->next) {
        if (self == handler) {
            found_handler = true;
        }

        if (found_handler) {
            ei_check(rc = clog_handler_annotation(handler, msg, key, value));
        }
    }

    for (handler = process_stack; handler != NULL; handler = handler->next) {
        if (self == handler) {
            found_handler = true;
        }

        if (found_handler) {
            ei_check(rc = clog_handler_annotation(handler, msg, key, value));
        }
    }

    return 0;

error:
    if (rc == CLOG_SKIP) {
        return 0;
    } else {
        return rc;
    }
}


void
clog_set_minimum_level(enum clog_level level)
{
    clog_minimum_level = level;
}

void
_clog_log_channel(enum clog_level level, const char *channel,
                  const char *format, ...)
{
    /* Otherwise create a clog_message object and pass it off to all of the
     * handlers. */
    struct clog_message  msg;
    msg.level = level;
    msg.channel = channel;
    msg.format = format;
    va_start(msg.args, format);
    clog_process_message(&msg);
    va_end(msg.args);
}

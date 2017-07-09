/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>

#include "clogger/api.h"
#include "clogger/handlers.h"

static int
clog_null_handler__annotation(struct clog_handler *self,
                              struct clog_message *msg,
                              const char *key, const char *value)
{
    return CLOG_SKIP;
}

static int
clog_null_handler__message(struct clog_handler *self, struct clog_message *msg)
{
    return CLOG_SKIP;
}

static void
clog_null_handler__free(struct clog_handler *self)
{
    cork_delete(struct clog_handler, self);
}

struct clog_handler *
clog_null_handler_new(void)
{
    struct clog_handler  *self = cork_new(struct clog_handler);
    self->annotation = clog_null_handler__annotation;
    self->message = clog_null_handler__message;
    self->free = clog_null_handler__free;
    return self;
}

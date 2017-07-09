/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/helpers/errors.h>

#include "clogger/api.h"
#include "clogger/handlers.h"


struct clog_keep_filter {
    struct clog_handler  parent;
    struct cork_hash_table  *channels;
};


static int
clog_keep_filter_check_channel(struct clog_keep_filter *filter,
                               const char *channel)
{
    if (cork_hash_table_get(filter->channels, (void *) channel) == NULL) {
        return CLOG_SKIP;
    } else {
        return CLOG_CONTINUE;
    }
}

static int
clog_keep_filter__annotation(struct clog_handler *handler,
                             struct clog_message *msg, const char *key,
                             const char *value)
{
    struct clog_keep_filter  *filter =
        cork_container_of(handler, struct clog_keep_filter, parent);
    return clog_keep_filter_check_channel(filter, msg->channel);
}

static int
clog_keep_filter__message(struct clog_handler *handler,
                          struct clog_message *msg)
{
    struct clog_keep_filter  *filter =
        cork_container_of(handler, struct clog_keep_filter, parent);
    return clog_keep_filter_check_channel(filter, msg->channel);
}

static void
clog_keep_filter__free(struct clog_handler *handler)
{
    struct clog_keep_filter  *filter =
        cork_container_of(handler, struct clog_keep_filter, parent);
    cork_hash_table_free(filter->channels);
    cork_delete(struct clog_keep_filter, filter);
}

struct clog_keep_filter *
clog_keep_filter_new(void)
{
    struct clog_keep_filter  *filter = cork_new(struct clog_keep_filter);
    filter->parent.annotation = clog_keep_filter__annotation;
    filter->parent.message = clog_keep_filter__message;
    filter->parent.free = clog_keep_filter__free;
    filter->channels = cork_string_hash_table_new(0, 0);
    cork_hash_table_set_free_key(filter->channels, (cork_free_f) cork_strfree);
    return filter;
}

void
clog_keep_filter_free(struct clog_keep_filter *filter)
{
    clog_keep_filter__free(&filter->parent);
}

void
clog_keep_filter_add(struct clog_keep_filter *filter, const char *channel)
{
    bool  is_new;
    struct cork_hash_table_entry  *entry;

    entry = cork_hash_table_get_or_create
        (filter->channels, (char *) channel, &is_new);
    if (is_new) {
        const char  *copy = cork_strdup(channel);
        entry->key = (char *) copy;
        entry->value = (char *) copy;
    }
}

void
clog_keep_filter_add_many(struct clog_keep_filter *filter, const char *str)
{
    struct cork_buffer  buf = CORK_BUFFER_INIT();
    const char  *end;

    while ((end = strchr(str, ',')) != NULL) {
        cork_buffer_set(&buf, str, end - str);
        clog_keep_filter_add(filter, buf.buf);
        str = end + 1;
    }

    cork_buffer_set_string(&buf, str);
    clog_keep_filter_add(filter, buf.buf);
    cork_buffer_done(&buf);
}

struct clog_handler *
clog_keep_filter_handler(struct clog_keep_filter *filter)
{
    return &filter->parent;
}

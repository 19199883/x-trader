/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef BOWSPRIT_H
#define BOWSPRIT_H

#include <stdio.h>

#include <clogger.h>
#include <libcork/core.h>
#include <libcork/threads.h>


/*-----------------------------------------------------------------------
 * Context
 */

struct bws_ctx {
    const char  *hostname;
};

struct bws_ctx *
bws_ctx_new(const char *hostname);

void
bws_ctx_free(struct bws_ctx *ctx);


/*-----------------------------------------------------------------------
 * Plugin
 */

struct bws_plugin {
    struct bws_ctx  *ctx;
    const char  *name;
    const char  *instance;
};

/* instance can be NULL */
struct bws_plugin *
bws_plugin_new(struct bws_ctx *ctx, const char *name, const char *instance);


/*-----------------------------------------------------------------------
 * derive
 */

struct bws_derive {
    uint64_t  value;
};

CORK_ATTR_UNUSED
static uint64_t
bws_derive_add(struct bws_derive *derive, uint64_t delta)
{
    return (derive->value += delta);
}

CORK_ATTR_UNUSED
static uint64_t
bws_derive_inc(struct bws_derive *derive)
{
    return bws_derive_add(derive, 1);
}

CORK_ATTR_UNUSED
static uint64_t
bws_derive_get(const struct bws_derive *derive)
{
    return derive->value;
}

CORK_ATTR_UNUSED
static void
bws_derive_set(struct bws_derive *derive, uint64_t value)
{
    derive->value = value;
}


/*-----------------------------------------------------------------------
 * Gauge
 */

struct bws_gauge {
    uint64_t  value;
};

CORK_ATTR_UNUSED
static uint64_t
bws_gauge_add(struct bws_gauge *gauge, uint64_t delta)
{
    return (gauge->value += delta);
}

CORK_ATTR_UNUSED
static uint64_t
bws_gauge_sub(struct bws_gauge *gauge, uint64_t delta)
{
    return (gauge->value -= delta);
}

CORK_ATTR_UNUSED
static uint64_t
bws_gauge_inc(struct bws_gauge *gauge)
{
    return bws_gauge_add(gauge, 1);
}

CORK_ATTR_UNUSED
static uint64_t
bws_gauge_dec(struct bws_gauge *gauge)
{
    return bws_gauge_sub(gauge, 1);
}

CORK_ATTR_UNUSED
static uint64_t
bws_gauge_get(const struct bws_gauge *gauge)
{
    return gauge->value;
}

CORK_ATTR_UNUSED
static void
bws_gauge_set(struct bws_gauge *gauge, uint64_t value)
{
    gauge->value = value;
}


/*-----------------------------------------------------------------------
 * Measurement
 */

struct bws_measurement {
    struct bws_plugin  *plugin;
    const char  *type_name;
    const char  *type_instance;
};

/* type_instance can be NULL */
struct bws_measurement *
bws_measurement_new(struct bws_plugin *plugin,
                    const char *type_name, const char *type_instance);

struct bws_derive *
bws_measurement_add_derive(struct bws_measurement *measurement,
                          const char *name);

struct bws_gauge *
bws_measurement_add_gauge(struct bws_measurement *measurement,
                          const char *name);


/* type_instance can be NULL */
struct bws_derive *
bws_derive_new(struct bws_plugin *plugin,
               const char *type_name, const char *type_instance);

/* type_instance can be NULL */
struct bws_gauge *
bws_gauge_new(struct bws_plugin *plugin,
              const char *type_name, const char *type_instance);


/*-----------------------------------------------------------------------
 * Measurement snapshots
 */

enum bws_value_kind {
    BWS_ABSOLUTE,
    BWS_COUNTER,
    BWS_DERIVE,
    BWS_GAUGE
};

struct bws_value_snapshot {
    struct bws_measurement  *measurement;
    uint64_t  value;
    const char  *name;
    enum bws_value_kind  kind;
};


struct bws_snapshot {
    size_t  count;
    struct bws_value_snapshot  *values;
};

struct bws_snapshot *
bws_snapshot_new(void);

void
bws_snapshot_free(struct bws_snapshot *snapshot);

void
bws_snapshot_resize(struct bws_snapshot *snapshot, size_t count);


void
bws_ctx_snapshot(struct bws_ctx *ctx, struct bws_snapshot *dest);

void
bws_plugin_snapshot(struct bws_plugin *plugin, struct bws_snapshot *dest);

void
bws_snapshot_filter_by_type_name(struct bws_snapshot *src,
                                 const char *type_name,
                                 struct bws_snapshot *dest);

void
bws_snapshot_sort(struct bws_snapshot *snapshot);


/*-----------------------------------------------------------------------
 * Rendering
 */

size_t
bws_snapshot_max_width(struct bws_snapshot *snapshot);

void
bws_value_snapshot_render_to_buffer(struct bws_value_snapshot *value,
                                    struct cork_buffer *dest, int max_width);

void
bws_value_snapshot_render_delta_to_buffer(struct bws_value_snapshot *curr,
                                          struct bws_value_snapshot *prev,
                                          struct cork_buffer *dest,
                                          unsigned int interval_sec,
                                          int max_width);

void
bws_snapshot_render_to_buffer(struct bws_snapshot *snapshot,
                              struct cork_buffer *dest);

void
bws_snapshot_render_delta_to_buffer(struct bws_snapshot *curr,
                                    struct bws_snapshot *prev,
                                    struct cork_buffer *dest,
                                    unsigned int interval_sec);

void
bws_snapshot_render_to_stream(struct bws_snapshot *snapshot, FILE *dest);

void
bws_snapshot_render_delta_to_stream(struct bws_snapshot *curr,
                                    struct bws_snapshot *prev,
                                    FILE *dest, unsigned int rate_interval_sec);


/*-----------------------------------------------------------------------
 * Periodic tasks
 */

struct bws_periodic;

typedef int
(*bws_periodic_run_f)(void *user_data, struct bws_snapshot *curr,
                      struct bws_snapshot *prev, cork_timestamp now);

struct bws_periodic *
bws_periodic_new(struct bws_ctx *ctx,
                 void *user_data, cork_free_f free_user_data,
                 bws_periodic_run_f run);

void
bws_periodic_free(struct bws_periodic *periodic);

unsigned int /* seconds */
bws_periodic_interval(struct bws_periodic *periodic);

/* 0 means to use the default, which is currently 30 seconds */
void
bws_periodic_set_interval(struct bws_periodic *periodic, unsigned int seconds);

int
bws_periodic_poll(struct bws_periodic *periodic);

/* Useful for test cases */
int
bws_periodic_mocked_poll(struct bws_periodic *periodic, cork_timestamp now);


int
bws_periodic_run_in_background(struct bws_periodic *periodic);

int
bws_periodic_join(struct bws_periodic *periodic);


/*-----------------------------------------------------------------------
 * Logging
 */

struct bws_periodic *
bws_logger_new(struct bws_ctx *ctx, const char *channel, enum clog_level level);


/*-----------------------------------------------------------------------
 * Rotation
 */

struct bws_rotated_files;

struct bws_rotated_files *
bws_rotated_files_new(struct bws_ctx *ctx, const char *output_path);

void
bws_rotated_files_free(struct bws_rotated_files *rotated);

struct bws_periodic *
bws_rotated_files_periodic(struct bws_rotated_files *rotated);

void
bws_rotated_files_set_file_duration(struct bws_rotated_files *rotated,
                                    unsigned int minutes);

void
bws_rotated_files_set_filename_slug(struct bws_rotated_files *rotated,
                                    const char *slug);


#endif /* BOWSPRIT_H */

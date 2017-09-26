/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>

#include <libcork/core.h>
#include <libcork/ds.h>

#include "bowsprit.h"


/*-----------------------------------------------------------------------
 * Common stuffs
 */

size_t
bws_snapshot_max_width(struct bws_snapshot *snapshot)
{
    size_t  i;
    size_t  overall_max = 0;
    for (i = 0; i < snapshot->count; i++) {
        size_t  value = snapshot->values[i].value;
        if (value > overall_max) {
            overall_max = value;
        }
    }
    return snprintf(NULL, 0, "%zu", overall_max);
}


/*-----------------------------------------------------------------------
 * Buffer renderer
 */

void
bws_value_snapshot_render_to_buffer(struct bws_value_snapshot *value,
                                    struct cork_buffer *dest, int width)
{
    struct bws_measurement  *measurement = value->measurement;
    struct bws_plugin  *plugin = measurement->plugin;

    cork_buffer_append_printf
        (dest, "%*" PRIu64 " %s", width, value->value, plugin->name);
    if (plugin->instance != NULL) {
        cork_buffer_append_printf(dest, "-%s", plugin->instance);
    }
    cork_buffer_append_printf(dest, "/%s", measurement->type_name);
    if (measurement->type_instance != NULL) {
        cork_buffer_append_printf(dest, "-%s", measurement->type_instance);
    }
    if (strcmp(value->name, "value") != 0) {
        cork_buffer_append_printf(dest, "/%s", value->name);
    }
}

void
bws_snapshot_render_to_buffer(struct bws_snapshot *snapshot,
                              struct cork_buffer *dest)
{
    size_t  i;
    int  width = bws_snapshot_max_width(snapshot);
    for (i = 0; i < snapshot->count; i++) {
        struct bws_value_snapshot  *value = &snapshot->values[i];
        bws_value_snapshot_render_to_buffer(value, dest, width);
        cork_buffer_append_literal(dest, "\n");
    }
}


#define  KILO  (1000.0)
#define  MEGA  (1000000.0)
#define  GIGA  (1000000000.0)
#define  TERA  (1000000000000.0)
#define  PETA  (1000000000000000.0)

static void
bws_render_rate(struct cork_buffer *dest, double rate_per_sec)
{
    if (rate_per_sec < KILO) {
        cork_buffer_append_printf(dest, "%8.3f  /s ", rate_per_sec);
    } else if (rate_per_sec < MEGA) {
        double  kilo_per_sec = rate_per_sec / KILO;
        cork_buffer_append_printf(dest, "%8.3f k/s ", kilo_per_sec);
    } else if (rate_per_sec < GIGA) {
        double  mega_per_sec = rate_per_sec / MEGA;
        cork_buffer_append_printf(dest, "%8.3f M/s ", mega_per_sec);
    } else if (rate_per_sec < TERA) {
        double  giga_per_sec = rate_per_sec / GIGA;
        cork_buffer_append_printf(dest, "%8.3f G/s ", giga_per_sec);
    } else if (rate_per_sec < PETA) {
        double  tera_per_sec = rate_per_sec / TERA;
        cork_buffer_append_printf(dest, "%8.3f T/s ", tera_per_sec);
    } else {
        double  peta_per_sec = rate_per_sec / PETA;
        cork_buffer_append_printf(dest, "%8.3f P/s ", peta_per_sec);
    }
}

static void
bws_render_empty_rate(struct cork_buffer *dest)
{
    cork_buffer_append_literal(dest, "             ");
}

void
bws_value_snapshot_render_delta_to_buffer(struct bws_value_snapshot *curr,
                                          struct bws_value_snapshot *prev,
                                          struct cork_buffer *dest,
                                          unsigned int rate_interval_sec,
                                          int width)
{
    struct bws_measurement  *measurement = curr->measurement;
    struct bws_plugin  *plugin = measurement->plugin;

    assert(prev == NULL || curr->measurement == prev->measurement);

    cork_buffer_append_printf(dest, "%*" PRIu64 " ", width, curr->value);
    if (curr->kind == BWS_DERIVE && prev != NULL) {
        double  difference = curr->value - prev->value;
        double  rate_per_sec = difference / rate_interval_sec;
        bws_render_rate(dest, rate_per_sec);
    } else {
        bws_render_empty_rate(dest);
    }

    cork_buffer_append_string(dest, plugin->name);
    if (plugin->instance != NULL) {
        cork_buffer_append_printf(dest, "-%s", plugin->instance);
    }
    cork_buffer_append_printf(dest, "/%s", measurement->type_name);
    if (measurement->type_instance != NULL) {
        cork_buffer_append_printf(dest, "-%s", measurement->type_instance);
    }
    if (strcmp(curr->name, "value") != 0) {
        cork_buffer_append_printf(dest, "/%s", curr->name);
    }
}

void
bws_snapshot_render_delta_to_buffer(struct bws_snapshot *curr,
                                    struct bws_snapshot *prev,
                                    struct cork_buffer *dest,
                                    unsigned int rate_interval_sec)
{
    size_t  i;
    int  width;

    assert(prev == NULL || curr->count == prev->count);
    width = bws_snapshot_max_width(curr);
    if (prev == NULL) {
        for (i = 0; i < curr->count; i++) {
            struct bws_value_snapshot  *curr_value = &curr->values[i];
            bws_value_snapshot_render_delta_to_buffer
                (curr_value, NULL, dest, rate_interval_sec, width);
            cork_buffer_append_literal(dest, "\n");
        }
    } else {
        for (i = 0; i < curr->count; i++) {
            struct bws_value_snapshot  *curr_value = &curr->values[i];
            struct bws_value_snapshot  *prev_value = &prev->values[i];
            bws_value_snapshot_render_delta_to_buffer
                (curr_value, prev_value, dest, rate_interval_sec, width);
            cork_buffer_append_literal(dest, "\n");
        }
    }
}


/*-----------------------------------------------------------------------
 * Stream renderer
 */

void
bws_snapshot_render_to_stream(struct bws_snapshot *snapshot, FILE *dest)
{
    struct cork_buffer  buf = CORK_BUFFER_INIT();
    bws_snapshot_render_to_buffer(snapshot, &buf);
    fwrite(buf.buf, 1, buf.size, dest);
    cork_buffer_done(&buf);
}

void
bws_snapshot_render_delta_to_stream(struct bws_snapshot *curr,
                                    struct bws_snapshot *prev,
                                    FILE *dest, unsigned int rate_interval_sec)
{
    struct cork_buffer  buf = CORK_BUFFER_INIT();
    bws_snapshot_render_delta_to_buffer(curr, prev, &buf, rate_interval_sec);
    fwrite(buf.buf, 1, buf.size, dest);
    cork_buffer_done(&buf);
}

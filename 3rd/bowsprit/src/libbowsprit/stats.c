/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <libcork/core.h>
#include <libcork/threads.h>

#include "bowsprit.h"


#define add_to_list(head, element) \
    do { \
        do { \
            old_head = (head); \
            (element)->next = old_head; \
        } while (CORK_UNLIKELY(cork_ptr_cas(&(head), old_head, (element)) \
                               != old_head)); \
    } while (0)

/*-----------------------------------------------------------------------
 * Measured values
 */

struct bws_value {
    struct bws_value  *next;
    const char  *name;
    union {
        struct bws_derive  derive;
        struct bws_gauge  gauge;
    } _;
    enum bws_value_kind  kind;
};

static struct bws_value *
bws_derive_value_new(const char *name)
{
    struct bws_value *value = cork_new(struct bws_value);
    value->name = cork_strdup(name);
    value->kind = BWS_DERIVE;
    value->_.derive.value = 0;
    return value;
}

static struct bws_value *
bws_gauge_value_new(const char *name)
{
    struct bws_value *value = cork_new(struct bws_value);
    value->name = cork_strdup(name);
    value->kind = BWS_GAUGE;
    value->_.gauge.value = 0;
    return value;
}

static void
bws_value_free(struct bws_value *value)
{
    cork_strfree(value->name);
    cork_delete(struct bws_value, value);
}

static void
bws_value_snapshot(struct bws_measurement *measurement, struct bws_value *value,
                   struct bws_value_snapshot *dest)
{
    dest->measurement = measurement;
    dest->name = value->name;
    dest->kind = value->kind;
    switch (value->kind) {
        case BWS_DERIVE:
            dest->value = value->_.derive.value;
            break;
        case BWS_GAUGE:
            dest->value = value->_.gauge.value;
            break;
        default:
            break;
    }
}


/*-----------------------------------------------------------------------
 * Measurements
 */

struct bws_measurement_priv {
    struct bws_measurement  public;
    volatile size_t  value_count;
    struct bws_value * volatile  values;
    struct bws_measurement_priv  *next;
};

static struct bws_measurement_priv *
bws_measurement_priv_new(struct bws_plugin *pplugin,
                         const char *type_name, const char *type_instance)
{
    struct bws_measurement_priv  *measurement =
        cork_new(struct bws_measurement_priv);
    measurement->public.plugin = pplugin;
    measurement->public.type_name = cork_strdup(type_name);
    measurement->public.type_instance =
        (type_instance == NULL)? NULL: cork_strdup(type_instance);
    measurement->value_count = 0;
    measurement->values = NULL;
    return measurement;
}

static void
bws_measurement_priv_free(struct bws_measurement_priv *measurement)
{
    struct bws_value  *curr;
    struct bws_value  *next;
    for (curr = measurement->values; curr != NULL; curr = next) {
        next = curr->next;
        bws_value_free(curr);
    }
    cork_strfree(measurement->public.type_name);
    if (measurement->public.type_instance != NULL) {
        cork_strfree(measurement->public.type_instance);
    }
    cork_delete(struct bws_measurement_priv, measurement);
}

struct bws_derive *
bws_measurement_add_derive(struct bws_measurement *pmeasurement,
                           const char *name)
{
    struct bws_measurement_priv  *measurement =
        cork_container_of(pmeasurement, struct bws_measurement_priv, public);
    struct bws_value  *value = bws_derive_value_new(name);
    struct bws_value  *old_head;
    add_to_list(measurement->values, value);
    cork_size_atomic_add(&measurement->value_count, 1);
    return &value->_.derive;
}

struct bws_gauge *
bws_measurement_add_gauge(struct bws_measurement *pmeasurement,
                          const char *name)
{
    struct bws_measurement_priv  *measurement =
        cork_container_of(pmeasurement, struct bws_measurement_priv, public);
    struct bws_value  *value = bws_gauge_value_new(name);
    struct bws_value  *old_head;
    add_to_list(measurement->values, value);
    cork_size_atomic_add(&measurement->value_count, 1);
    return &value->_.gauge;
}


/*-----------------------------------------------------------------------
 * Plugins
 */

struct bws_plugin_priv {
    struct bws_plugin  public;
    struct bws_measurement_priv * volatile  measurements;
    struct bws_plugin_priv  *next;
};

static struct bws_plugin_priv *
bws_plugin_priv_new(struct bws_ctx *ctx, const char *name, const char *instance)
{
    struct bws_plugin_priv  *plugin = cork_new(struct bws_plugin_priv);
    plugin->public.ctx = ctx;
    plugin->public.name = cork_strdup(name);
    plugin->public.instance = (instance == NULL)? NULL: cork_strdup(instance);
    plugin->measurements = NULL;
    return plugin;
}

static void
bws_plugin_priv_free(struct bws_plugin_priv *plugin)
{
    struct bws_measurement_priv  *curr;
    struct bws_measurement_priv  *next;
    for (curr = plugin->measurements; curr != NULL; curr = next) {
        next = curr->next;
        bws_measurement_priv_free(curr);
    }
    cork_strfree(plugin->public.name);
    if (plugin->public.instance != NULL) {
        cork_strfree(plugin->public.instance);
    }
    cork_delete(struct bws_plugin_priv, plugin);
}

struct bws_measurement *
bws_measurement_new(struct bws_plugin *pplugin,
                    const char *type_name, const char *type_instance)
{
    struct bws_plugin_priv  *plugin =
        cork_container_of(pplugin, struct bws_plugin_priv, public);
    struct bws_measurement_priv  *measurement =
        bws_measurement_priv_new(pplugin, type_name, type_instance);
    struct bws_measurement_priv  *old_head;
    add_to_list(plugin->measurements, measurement);
    return &measurement->public;
}

struct bws_derive *
bws_derive_new(struct bws_plugin *plugin,
               const char *type_name, const char *type_instance)
{
    struct bws_measurement  *measurement =
        bws_measurement_new(plugin, type_name, type_instance);
    return bws_measurement_add_derive(measurement, "value");
}

struct bws_gauge *
bws_gauge_new(struct bws_plugin *plugin,
              const char *type_name, const char *type_instance)
{
    struct bws_measurement  *measurement =
        bws_measurement_new(plugin, type_name, type_instance);
    return bws_measurement_add_gauge(measurement, "value");
}

void
bws_plugin_snapshot(struct bws_plugin *pplugin, struct bws_snapshot *dest)
{
    struct bws_plugin_priv  *plugin =
        cork_container_of(pplugin, struct bws_plugin_priv, public);
    size_t  count;
    struct bws_measurement_priv  *measurement;
    struct bws_value_snapshot  *snap_value;

    count = 0;
    for (measurement = plugin->measurements; measurement != NULL;
         measurement = measurement->next) {
        count += measurement->value_count;
    }

    bws_snapshot_resize(dest, count);

    snap_value = dest->values;
    for (measurement = plugin->measurements; measurement != NULL;
         measurement = measurement->next) {
        struct bws_value  *value;
        for (value = measurement->values; value != NULL; value = value->next) {
            bws_value_snapshot(&measurement->public, value, snap_value++);
        }
    }
}


/*-----------------------------------------------------------------------
 * Contexts
 */

struct bws_ctx_priv {
    struct bws_ctx  public;
    struct bws_plugin_priv * volatile  plugins;
};

struct bws_ctx *
bws_ctx_new(const char *hostname)
{
    struct bws_ctx_priv  *ctx = cork_new(struct bws_ctx_priv);
    ctx->public.hostname = cork_strdup(hostname);
    ctx->plugins = NULL;
    return &ctx->public;
}

void
bws_ctx_free(struct bws_ctx *pctx)
{
    struct bws_ctx_priv  *ctx =
        cork_container_of(pctx, struct bws_ctx_priv, public);
    struct bws_plugin_priv  *curr;
    struct bws_plugin_priv  *next;
    for (curr = ctx->plugins; curr != NULL; curr = next) {
        next = curr->next;
        bws_plugin_priv_free(curr);
    }
    cork_strfree(ctx->public.hostname);
    cork_delete(struct bws_ctx_priv, ctx);
}

struct bws_plugin *
bws_plugin_new(struct bws_ctx *pctx, const char *name, const char *instance)
{
    struct bws_ctx_priv  *ctx =
        cork_container_of(pctx, struct bws_ctx_priv, public);
    struct bws_plugin_priv  *plugin = bws_plugin_priv_new(pctx, name, instance);
    struct bws_plugin_priv  *old_head;
    add_to_list(ctx->plugins, plugin);
    return &plugin->public;
}

void
bws_ctx_snapshot(struct bws_ctx *pctx, struct bws_snapshot *dest)
{
    struct bws_ctx_priv  *ctx =
        cork_container_of(pctx, struct bws_ctx_priv, public);
    size_t  count;
    struct bws_plugin_priv  *plugin;
    struct bws_value_snapshot  *snap_value;

    count = 0;
    for (plugin = ctx->plugins; plugin != NULL; plugin = plugin->next) {
        struct bws_measurement_priv  *measurement;
        for (measurement = plugin->measurements; measurement != NULL;
             measurement = measurement->next) {
            count += measurement->value_count;
        }
    }

    bws_snapshot_resize(dest, count);

    snap_value = dest->values;
    for (plugin = ctx->plugins; plugin != NULL; plugin = plugin->next) {
        struct bws_measurement_priv  *measurement;
        for (measurement = plugin->measurements; measurement != NULL;
             measurement = measurement->next) {
            struct bws_value  *value;
            for (value = measurement->values; value != NULL;
                 value = value->next) {
                bws_value_snapshot(&measurement->public, value, snap_value++);
            }
        }
    }
}

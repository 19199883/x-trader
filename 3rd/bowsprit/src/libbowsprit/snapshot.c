/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>

#include <libcork/core.h>

#include "bowsprit.h"


/*-----------------------------------------------------------------------
 * Measurement snapshots
 */

struct bws_snapshot_priv {
    struct bws_snapshot  public;
    size_t  allocated_count;
    size_t  allocated_size;
};

struct bws_snapshot *
bws_snapshot_new(void)
{
    struct bws_snapshot_priv  *snapshot = cork_new(struct bws_snapshot_priv);
    snapshot->public.count = 0;
    snapshot->public.values = NULL;
    snapshot->allocated_count = 0;
    snapshot->allocated_size = 0;
    return &snapshot->public;
}

void
bws_snapshot_free(struct bws_snapshot *psnapshot)
{
    struct bws_snapshot_priv  *snapshot =
        cork_container_of(psnapshot, struct bws_snapshot_priv, public);
    if (snapshot->allocated_size != 0) {
        cork_free(snapshot->public.values, snapshot->allocated_size);
    }
    cork_delete(struct bws_snapshot_priv, snapshot);
}

void
bws_snapshot_resize(struct bws_snapshot *psnapshot, size_t count)
{
    struct bws_snapshot_priv  *snapshot =
        cork_container_of(psnapshot, struct bws_snapshot_priv, public);
    snapshot->public.count = count;
    if (CORK_UNLIKELY(snapshot->allocated_count == 0 && count != 0)) {
        size_t  size = count * sizeof(struct bws_value_snapshot);
        snapshot->public.values = cork_malloc(size);
        snapshot->allocated_count = count;
        snapshot->allocated_size = size;
    } else if (CORK_UNLIKELY(count > snapshot->allocated_count)) {
        size_t  new_count = snapshot->allocated_count * 2;
        size_t  new_size = new_count * sizeof(struct bws_value_snapshot);
        snapshot->public.values = cork_realloc
            (snapshot->public.values, snapshot->allocated_size, new_size);
        snapshot->allocated_count = new_count;
        snapshot->allocated_size = new_size;
    }
}


/*-----------------------------------------------------------------------
 * Filtering
 */

void
bws_snapshot_filter_by_type_name(struct bws_snapshot *src,
                                 const char *type_name,
                                 struct bws_snapshot *dest)
{
    size_t  i;
    size_t  j;
    size_t  filtered_count = 0;

    for (i = 0; i < src->count; i++) {
        if (strcmp(type_name, src->values[i].measurement->type_name) == 0) {
            filtered_count++;
        }
    }

    bws_snapshot_resize(dest, filtered_count);

    for (i = 0, j = 0; i < src->count; i++) {
        if (strcmp(type_name, src->values[i].measurement->type_name) == 0) {
            dest->values[j++] = src->values[i];
        }
    }
}


/*-----------------------------------------------------------------------
 * Sorting
 */

/* Sort descending by current value.  qsort always does an ascending sort, so we
 * need to reverse the -1/0/1 return value to get a descending sort. */
static int
compare_value(const void *ventry1, const void *ventry2)
{
    const struct bws_value_snapshot  *entry1 = ventry1;
    const struct bws_value_snapshot  *entry2 = ventry2;
    size_t  value1 = entry1->value;
    size_t  value2 = entry2->value;
    if (value1 < value2) {
        return 1;
    } else if (value1 > value2) {
        return -1;
    } else {
        return 0;
    }
}

void
bws_snapshot_sort(struct bws_snapshot *snapshot)
{
    qsort(snapshot->values, snapshot->count,
          sizeof(struct bws_value_snapshot), compare_value);
}

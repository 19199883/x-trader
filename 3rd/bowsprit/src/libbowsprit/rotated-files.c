/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include <clogger.h>
#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/os.h>
#include <libcork/helpers/errors.h>
#include <libcork/helpers/posix.h>

#include "bowsprit.h"

#define CLOG_CHANNEL  "bowsprit"


/*-----------------------------------------------------------------------
 * File rotation
 */

#define DEFAULT_FILE_DURATION_MIN  10

struct bws_rotated_files {
    struct bws_periodic  *periodic;
    struct cork_buffer  buf;
    struct cork_path  *output_path;
    struct cork_path  *temp_filename;
    struct cork_path  *real_filename;
    cork_timestamp  file_duration;
    cork_timestamp  file_expiration;
    FILE  *out;
    const char  *filename_slug;
    size_t  sequence;
};

static int
bws_rotated_files_open_file(struct bws_rotated_files *rotated,
                            cork_timestamp now)
{
    cork_buffer_clear(&rotated->buf);
    cork_buffer_append_literal(&rotated->buf, ".stats");
    if (rotated->filename_slug != NULL) {
        cork_buffer_append_literal(&rotated->buf, "-");
        cork_buffer_append_string(&rotated->buf, rotated->filename_slug);
    }
    cork_timestamp_format_utc
        (now, "-%Y%m%d-%H%M%S", &rotated->buf);
    cork_buffer_append_printf
        (&rotated->buf, "-%04zu.log.tmp", rotated->sequence);
    rotated->temp_filename =
        cork_path_join(rotated->output_path, rotated->buf.buf);

    cork_buffer_clear(&rotated->buf);
    cork_buffer_append_literal(&rotated->buf, "stats");
    if (rotated->filename_slug != NULL) {
        cork_buffer_append_literal(&rotated->buf, "-");
        cork_buffer_append_string(&rotated->buf, rotated->filename_slug);
    }
    cork_timestamp_format_utc
        (now, "-%Y%m%d-%H%M%S", &rotated->buf);
    cork_buffer_append_printf
        (&rotated->buf, "-%04zu.log", rotated->sequence);
    rotated->real_filename =
        cork_path_join(rotated->output_path, rotated->buf.buf);

    clog_debug("Open statistics file %s",
               cork_path_get(rotated->real_filename));
    ep_check_posix(rotated->out =
                   fopen(cork_path_get(rotated->temp_filename), "w"));
    rotated->file_expiration = now + rotated->file_duration;
    rotated->sequence++;
    return 0;

error:
    cork_path_free(rotated->temp_filename);
    cork_path_free(rotated->real_filename);
    return -1;
}

static int
bws_rotated_files_close_file(struct bws_rotated_files *rotated)
{
    clog_debug("Close statistics file %s",
               cork_path_get(rotated->real_filename));
    ei_check_posix(fclose(rotated->out));
    ei_check_posix(rename(cork_path_get(rotated->temp_filename),
                          cork_path_get(rotated->real_filename)));
    cork_path_free(rotated->temp_filename);
    cork_path_free(rotated->real_filename);
    return 0;

error:
    cork_path_free(rotated->temp_filename);
    cork_path_free(rotated->real_filename);
    return -1;
}

static int
bws_rotated_files__run(void *user_data, struct bws_snapshot *curr,
                       struct bws_snapshot *prev, cork_timestamp now)
{
    struct bws_rotated_files  *rotated = user_data;
    unsigned int  interval_sec = bws_periodic_interval(rotated->periodic);

    if (rotated->file_expiration == 0) {
        rii_check(bws_rotated_files_open_file(rotated, now));
    } else if (now >= rotated->file_expiration) {
        rii_check(bws_rotated_files_close_file(rotated));
        rii_check(bws_rotated_files_open_file(rotated, now));
    }

    cork_buffer_clear(&rotated->buf);
    cork_timestamp_format_utc(now, "%Y-%m-%d %H:%M:%S", &rotated->buf);
    clog_debug("Output statistics as of %s", (char *) rotated->buf.buf);
    fputs("=== Statistics of as of ", rotated->out);
    fwrite(rotated->buf.buf, 1, rotated->buf.size, rotated->out);
    putc('\n', rotated->out);

    cork_buffer_clear(&rotated->buf);
    bws_snapshot_render_delta_to_buffer
        (curr, prev, &rotated->buf, interval_sec);
    fwrite(rotated->buf.buf, 1, rotated->buf.size, rotated->out);
    rii_check_posix(fflush(rotated->out));
    return 0;
}

static void
bws_rotated_files__free(void *user_data)
{
    struct bws_rotated_files  *rotated = user_data;
    if (rotated->out != NULL) {
        bws_rotated_files_close_file(rotated);
    }
    if (rotated->filename_slug != NULL) {
        cork_strfree(rotated->filename_slug);
    }
    cork_path_free(rotated->output_path);
    cork_buffer_done(&rotated->buf);
    cork_delete(struct bws_rotated_files, rotated);
}

struct bws_rotated_files *
bws_rotated_files_new(struct bws_ctx *ctx, const char *output_path)
{
    struct bws_rotated_files  *rotated = cork_new(struct bws_rotated_files);
    cork_buffer_init(&rotated->buf);
    cork_timestamp_init_sec
        (&rotated->file_duration, DEFAULT_FILE_DURATION_MIN * 60);
    rotated->file_expiration = 0;
    rotated->output_path = cork_path_new(output_path);
    cork_path_set_absolute(rotated->output_path);
    rotated->out = NULL;
    rotated->filename_slug = NULL;
    rotated->sequence = 0;
    rotated->periodic = bws_periodic_new
        (ctx, rotated, bws_rotated_files__free, bws_rotated_files__run);
    return rotated;
}

void
bws_rotated_files_free(struct bws_rotated_files *rotated)
{
    bws_periodic_free(rotated->periodic);
}

struct bws_periodic *
bws_rotated_files_periodic(struct bws_rotated_files *rotated)
{
    return rotated->periodic;
}

void
bws_rotated_files_set_file_duration(struct bws_rotated_files *rotated,
                                    unsigned int minutes)
{
    cork_timestamp_init_sec(&rotated->file_duration, minutes * 60);
}

void
bws_rotated_files_set_filename_slug(struct bws_rotated_files *rotated,
                                    const char *slug)
{
    if (rotated->filename_slug != NULL) {
        cork_strfree(rotated->filename_slug);
    }
    rotated->filename_slug = cork_strdup(slug);
}

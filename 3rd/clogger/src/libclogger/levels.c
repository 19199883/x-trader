/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include "clogger/api.h"


const char *
clog_level_name(enum clog_level level)
{
    switch (level) {
        case CLOG_LEVEL_NONE:
            return "NONE";
        case CLOG_LEVEL_CRITICAL:
            return "CRITICAL";
        case CLOG_LEVEL_ERROR:
            return "ERROR";
        case CLOG_LEVEL_WARNING:
            return "WARNING";
        case CLOG_LEVEL_NOTICE:
            return "NOTICE";
        case CLOG_LEVEL_INFO:
            return "INFO";
        case CLOG_LEVEL_DEBUG:
            return "DEBUG";
        case CLOG_LEVEL_TRACE:
            return "TRACE";
        default:
            return "unknown level";
    }
}

const char *
clog_level_name_fixed_width(enum clog_level level)
{
    switch (level) {
        case CLOG_LEVEL_NONE:
            return "NONE    ";
        case CLOG_LEVEL_CRITICAL:
            return "CRITICAL";
        case CLOG_LEVEL_ERROR:
            return "ERROR   ";
        case CLOG_LEVEL_WARNING:
            return "WARNING ";
        case CLOG_LEVEL_NOTICE:
            return "NOTICE  ";
        case CLOG_LEVEL_INFO:
            return "INFO    ";
        case CLOG_LEVEL_DEBUG:
            return "DEBUG   ";
        case CLOG_LEVEL_TRACE:
            return "TRACE   ";
        default:
            return "unknown ";
    }
}

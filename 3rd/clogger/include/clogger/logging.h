/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef CLOGGER_LOGGING_H
#define CLOGGER_LOGGING_H

#include <libcork/core.h>


#define CLOG_DEFAULT_FORMAT  "[%L] %c: %m"

void
clog_set_default_format(const char *fmt);

/* Output will be sent to stderr.
 *
 * Environment variables:
 *
 * CLOG={CRITICAL|ERROR|WARNING|NOTICE|INFO|DEBUG|TRACE)
 *   sets the minimum log level
 *
 * CLOG_FORMAT=[format string]
 *   the format string to use
 *
 * CLOG_CHANNELS=[comma-separated list]
 *   channel names not in the list will be ignored
 */

int
clog_setup_logging(void);


#endif /* CLOGGER_LOGGING_H */

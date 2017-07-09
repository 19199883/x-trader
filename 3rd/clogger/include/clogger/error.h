/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef CLOGGER_ERROR_H
#define CLOGGER_ERROR_H

#include <libcork/core.h>


#define CLOG_BAD_CONFIG               0xffa63a2f
#define CLOG_BAD_FORMAT               0x12cd1404
#define CLOG_BAD_STACK                0x37d461f7

#define clog_bad_config(...) \
    cork_error_set_printf(CLOG_BAD_CONFIG, __VA_ARGS__)
#define clog_bad_format(...) \
    cork_error_set_printf(CLOG_BAD_FORMAT, __VA_ARGS__)
#define clog_bad_stack(...) \
    cork_error_set_printf(CLOG_BAD_STACK, __VA_ARGS__)


#endif /* CLOGGER_ERROR_H */

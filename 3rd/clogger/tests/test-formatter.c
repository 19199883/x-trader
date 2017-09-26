/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>

#include <libcork/core.h>
#include <libcork/ds.h>
#include <libcork/helpers/errors.h>

#include "clogger/formatter.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Format strings
 */

#define GOOD(str) \
    do { \
        struct clog_formatter  *fmt; \
        fail_if_error(fmt = clog_formatter_new(str)); \
        clog_formatter_free(fmt); \
    } while (0)

#define BAD(str) \
    do { \
        fail_unless_error(clog_formatter_new(str)); \
    } while (0)

START_TEST(test_format_parse_01)
{
    DESCRIBE_TEST;
    GOOD("");
    GOOD("##");
    GOOD("%%");
    GOOD("%l %L %c %m");
    GOOD("#{var}");
    GOOD("#!{var}{ %% %k %v}");
    GOOD("#*{ %% %k %v}");
    GOOD("test 1");
    GOOD("test ##1 %%1");
    BAD("#{unterminated var");
    BAD("#^");
    BAD("#!");
    BAD("#!{unterminated var");
    BAD("#!{var}");
    BAD("#!{var}{unterminated spec");
    BAD("#!{var}{%!}");
    BAD("#*");
    BAD("#*{unterminated spec");
    BAD("#*{%!}");
    BAD("%!");
}
END_TEST


/*-----------------------------------------------------------------------
 * Formatting results
 */

static int
test_message(struct clog_formatter *self, struct cork_buffer *dest,
             enum clog_level level, const char *channel, const char *fmt, ...)
{
    struct clog_message  msg;
    msg.level = level;
    msg.channel = channel;
    msg.format = fmt;
    va_start(msg.args, fmt);
    return clog_formatter_finish(self, &msg, dest);
}

START_TEST(test_format_01)
{
    DESCRIBE_TEST;
    struct clog_formatter  *fmt;
    struct cork_buffer  dest = CORK_BUFFER_INIT();
    const char  *fmt_str =
        "  hello #{var1} ## #{var2} "
        "[%l] [%L] %c %m "
        "#!{var1}{%k = %v }"
        "#!{var2}{%% }"
        "#!{var3}{%% }"
        "#*{%k=%v }"
        "world #{var1} #{var3}";
    const char  *expected =
        "  hello value1 # value2 "
        "[INFO] [INFO    ] test This is only a test. "
        "var1 = value1 "
        "% "
        ""
        "var1=value1 var2=value2 "
        "world value1 ";

    fail_if_error(fmt = clog_formatter_new(fmt_str));
    fail_if_error(clog_formatter_start(fmt));
    fail_if_error(clog_formatter_annotation(fmt, "var1", "value1"));
    fail_if_error(clog_formatter_annotation(fmt, "var2", "value2"));
    fail_if_error(test_message
                  (fmt, &dest, CLOG_LEVEL_INFO,
                   "test", "This is only a test."));

    ck_assert_str_eq((char *) dest.buf, expected);
    cork_buffer_done(&dest);
    clog_formatter_free(fmt);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("logging");

    TCase  *tc_format = tcase_create("format");
    tcase_add_test(tc_format, test_format_parse_01);
    tcase_add_test(tc_format, test_format_01);
    suite_add_tcase(s, tc_format);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    setup_allocator();
    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}

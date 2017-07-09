# -*- coding: utf-8 -*-
"""
    test_docutilsconf
    ~~~~~~~~~~~~~~~~~

    Test docutils.conf support for several writers.

    :copyright: Copyright 2007-2017 by the Sphinx team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import re

import pytest
from sphinx.testing.path import path
from sphinx.testing.util import SkipTest


def regex_count(expr, result):
    return len(re.findall(expr, result))


@pytest.mark.sphinx('html', testroot='docutilsconf', freshenv=True, docutilsconf='')
def test_html_with_default_docutilsconf(app, status, warning):
    app.builder.build(['contents'])
    result = (app.outdir / 'contents.html').text(encoding='utf-8')

    assert regex_count(r'<th class="field-name">', result) == 1
    assert regex_count(r'<th class="field-name" colspan="2">', result) == 1
    assert regex_count(r'<td class="option-group">', result) == 1
    assert regex_count(r'<td class="option-group" colspan="2">', result) == 1


@pytest.mark.sphinx('html', testroot='docutilsconf', freshenv=True, docutilsconf=(
    '\n[html4css1 writer]'
    '\noption-limit:1'
    '\nfield-name-limit:1'
    '\n')
)
def test_html_with_docutilsconf(app, status, warning):
    app.builder.build(['contents'])
    result = (app.outdir / 'contents.html').text(encoding='utf-8')

    assert regex_count(r'<th class="field-name">', result) == 0
    assert regex_count(r'<th class="field-name" colspan="2">', result) == 2
    assert regex_count(r'<td class="option-group">', result) == 0
    assert regex_count(r'<td class="option-group" colspan="2">', result) == 2


@pytest.mark.sphinx('html', testroot='docutilsconf')
def test_html(app, status, warning):
    app.builder.build(['contents'])
    assert warning.getvalue() == ''


@pytest.mark.sphinx('latex', testroot='docutilsconf')
def test_latex(app, status, warning):
    app.builder.build(['contents'])
    assert warning.getvalue() == ''


@pytest.mark.sphinx('man', testroot='docutilsconf')
def test_man(app, status, warning):
    app.builder.build(['contents'])
    assert warning.getvalue() == ''


@pytest.mark.sphinx('texinfo', testroot='docutilsconf')
def test_texinfo(app, status, warning):
    app.builder.build(['contents'])


@pytest.mark.sphinx('html', testroot='docutilsconf',
                    docutilsconf='[general]\nsource_link=true\n')
def test_docutils_source_link_with_nonascii_file(app, status, warning):
    srcdir = path(app.srcdir)
    mb_name = u'\u65e5\u672c\u8a9e'
    try:
        (srcdir / (mb_name + '.txt')).write_text('')
    except UnicodeEncodeError:
        from sphinx.testing.path import FILESYSTEMENCODING
        raise SkipTest(
            'nonascii filename not supported on this filesystem encoding: '
            '%s', FILESYSTEMENCODING)

    app.builder.build_all()

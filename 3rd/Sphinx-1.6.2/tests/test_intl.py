# -*- coding: utf-8 -*-
"""
    test_intl
    ~~~~~~~~~

    Test message patching for internationalization purposes.  Runs the text
    builder in the test root.

    :copyright: Copyright 2007-2017 by the Sphinx team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""
from __future__ import print_function

import os
import re
import pickle
from docutils import nodes

from babel.messages import pofile, mofile
from six import string_types
import pytest

from sphinx.testing.util import (
    path, etree_parse, strip_escseq,
    assert_re_search, assert_not_re_search, assert_startswith, assert_node
)


sphinx_intl = pytest.mark.sphinx(
    testroot='intl',
    confoverrides={
        'language': 'xx', 'locale_dirs': ['.'],
        'gettext_compact': False,
    },
)


def read_po(pathname):
    with pathname.open() as f:
        return pofile.read_po(f)


def write_mo(pathname, po):
    with pathname.open('wb') as f:
        return mofile.write_mo(f, po)


@pytest.fixture
def build_mo():
    def builder(srcdir):
        """
        :param str srcdir: app.srcdir
        """
        srcdir = path(srcdir)
        for dirpath, dirs, files in os.walk(srcdir):
            dirpath = path(dirpath)
            for f in [f for f in files if f.endswith('.po')]:
                po = dirpath / f
                mo = srcdir / 'xx' / 'LC_MESSAGES' / (
                    os.path.relpath(po[:-3], srcdir) + '.mo')
                if not mo.parent.exists():
                    mo.parent.makedirs()

                write_mo(mo, read_po(po))
    return builder


@pytest.fixture(autouse=True)
def setup_intl(app_params, build_mo):
    build_mo(app_params.kwargs['srcdir'])


@pytest.fixture(autouse=True)
def _info(app):
    yield
    print('# language:', app.config.language)
    print('# locale_dirs:', app.config.locale_dirs)


def elem_gettexts(elem):
    def itertext(self):
        # this function copied from Python-2.7 'ElementTree.itertext'.
        # for compatibility to Python-2.6
        tag = self.tag
        if not isinstance(tag, string_types) and tag is not None:
            return
        if self.text:
            yield self.text
        for e in self:
            for s in itertext(e):
                yield s
            if e.tail:
                yield e.tail
    return [_f for _f in [s.strip() for s in itertext(elem)] if _f]


def elem_getref(elem):
    return elem.attrib.get('refid') or elem.attrib.get('refuri')


def assert_elem(elem, texts=None, refs=None, names=None):
    if texts is not None:
        _texts = elem_gettexts(elem)
        assert _texts == texts
    if refs is not None:
        _refs = [elem_getref(x) for x in elem.findall('reference')]
        assert _refs == refs
    if names is not None:
        _names = elem.attrib.get('names').split()
        assert _names == names


def assert_count(expected_expr, result, count):
    find_pair = (expected_expr, result)
    assert len(re.findall(*find_pair)) == count, find_pair


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_toctree(app):
    app.build()
    result = (app.outdir / 'contents.txt').text(encoding='utf-8')
    assert_startswith(result, u"CONTENTS\n********\n\nTABLE OF CONTENTS\n")


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_emit_warnings(app, warning):
    app.build()
    # test warnings in translation
    warnings = getwarning(warning)
    warning_expr = u'.*/warnings.txt:4: ' \
                   u'WARNING: Inline literal start-string without end-string.\n'
    assert_re_search(warning_expr, warnings)


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_warning_node(app):
    app.build()
    # test warnings in translation
    result = (app.outdir / 'warnings.txt').text(encoding='utf-8')
    expect = (u"I18N WITH REST WARNINGS"
              u"\n***********************\n"
              u"\nLINE OF >>``<<BROKEN LITERAL MARKUP.\n")
    assert result == expect


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_title_underline(app):
    app.build()
    # --- simple translation; check title underlines
    result = (app.outdir / 'bom.txt').text(encoding='utf-8')
    expect = (u"Datei mit UTF-8"
              u"\n***************\n"  # underline matches new translation
              u"\nThis file has umlauts: äöü.\n")
    assert result == expect


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_subdirs(app):
    app.build()
    # --- check translation in subdirs
    result = (app.outdir / 'subdir' / 'contents.txt').text(encoding='utf-8')
    assert_startswith(result, u"subdir contents\n***************\n")


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_inconsistency_warnings(app, warning):
    app.build()
    # --- check warnings for inconsistency in number of references
    result = (app.outdir / 'refs_inconsistency.txt').text(encoding='utf-8')
    expect = (u"I18N WITH REFS INCONSISTENCY"
              u"\n****************************\n"
              u"\n* FOR FOOTNOTE [ref2].\n"
              u"\n* reference FOR reference.\n"
              u"\n* ORPHAN REFERENCE: I18N WITH REFS INCONSISTENCY.\n"
              u"\n[1] THIS IS A AUTO NUMBERED FOOTNOTE.\n"
              u"\n[ref2] THIS IS A NAMED FOOTNOTE.\n"
              u"\n[100] THIS IS A NUMBERED FOOTNOTE.\n")
    assert result == expect

    warnings = getwarning(warning)
    warning_fmt = u'.*/refs_inconsistency.txt:\\d+: ' \
                  u'WARNING: inconsistent %s in translated message\n'
    expected_warning_expr = (
        warning_fmt % 'footnote references' +
        warning_fmt % 'references' +
        warning_fmt % 'references')
    assert_re_search(expected_warning_expr, warnings)


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_literalblock_warnings(app, warning):
    app.build()
    # --- check warning for literal block
    result = (app.outdir / 'literalblock.txt').text(encoding='utf-8')
    expect = (u"I18N WITH LITERAL BLOCK"
              u"\n***********************\n"
              u"\nCORRECT LITERAL BLOCK:\n"
              u"\n   this is"
              u"\n   literal block\n"
              u"\nMISSING LITERAL BLOCK:\n"
              u"\n<SYSTEM MESSAGE:")
    assert_startswith(result, expect)

    warnings = getwarning(warning)
    expected_warning_expr = u'.*/literalblock.txt:\\d+: ' \
                            u'WARNING: Literal block expected; none found.'
    assert_re_search(expected_warning_expr, warnings)


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_definition_terms(app):
    app.build()
    # --- definition terms: regression test for #975, #2198, #2205
    result = (app.outdir / 'definition_terms.txt').text(encoding='utf-8')
    expect = (u"I18N WITH DEFINITION TERMS"
              u"\n**************************\n"
              u"\nSOME TERM"
              u"\n   THE CORRESPONDING DEFINITION\n"
              u"\nSOME *TERM* WITH LINK"
              u"\n   THE CORRESPONDING DEFINITION #2\n"
              u"\nSOME **TERM** WITH : CLASSIFIER1 : CLASSIFIER2"
              u"\n   THE CORRESPONDING DEFINITION\n"
              u"\nSOME TERM WITH : CLASSIFIER[]"
              u"\n   THE CORRESPONDING DEFINITION\n"
              )
    assert result == expect


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_glossary_term(app, warning):
    app.build()
    # --- glossary terms: regression test for #1090
    result = (app.outdir / 'glossary_terms.txt').text(encoding='utf-8')
    expect = (u"I18N WITH GLOSSARY TERMS"
              u"\n************************\n"
              u"\nSOME NEW TERM"
              u"\n   THE CORRESPONDING GLOSSARY\n"
              u"\nSOME OTHER NEW TERM"
              u"\n   THE CORRESPONDING GLOSSARY #2\n"
              u"\nLINK TO *SOME NEW TERM*.\n")
    assert result == expect
    warnings = getwarning(warning)
    assert 'term not in glossary' not in warnings


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_glossary_term_inconsistencies(app, warning):
    app.build()
    # --- glossary term inconsistencies: regression test for #1090
    result = (app.outdir / 'glossary_terms_inconsistency.txt').text(encoding='utf-8')
    expect = (u"I18N WITH GLOSSARY TERMS INCONSISTENCY"
              u"\n**************************************\n"
              u"\n1. LINK TO *SOME NEW TERM*.\n")
    assert result == expect

    warnings = getwarning(warning)
    expected_warning_expr = (
        u'.*/glossary_terms_inconsistency.txt:\\d+: '
        u'WARNING: inconsistent term references in translated message\n')
    assert_re_search(expected_warning_expr, warnings)


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_seealso(app):
    app.build()
    # --- seealso
    result = (app.outdir / 'seealso.txt').text(encoding='utf-8')
    expect = (u"I18N WITH SEEALSO"
              u"\n*****************\n"
              u"\nSee also: SHORT TEXT 1\n"
              u"\nSee also: LONG TEXT 1\n"
              u"\nSee also: SHORT TEXT 2\n"
              u"\n  LONG TEXT 2\n")
    assert result == expect


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_figure_captions(app):
    app.build()
    # --- figure captions: regression test for #940
    result = (app.outdir / 'figure.txt').text(encoding='utf-8')
    expect = (u"I18N WITH FIGURE CAPTION"
              u"\n************************\n"
              u"\n   [image]MY CAPTION OF THE FIGURE\n"
              u"\n   MY DESCRIPTION PARAGRAPH1 OF THE FIGURE.\n"
              u"\n   MY DESCRIPTION PARAGRAPH2 OF THE FIGURE.\n"
              u"\n"
              u"\nFIGURE IN THE BLOCK"
              u"\n===================\n"
              u"\nBLOCK\n"
              u"\n      [image]MY CAPTION OF THE FIGURE\n"
              u"\n      MY DESCRIPTION PARAGRAPH1 OF THE FIGURE.\n"
              u"\n      MY DESCRIPTION PARAGRAPH2 OF THE FIGURE.\n"
              u"\n"
              u"\n"
              u"IMAGE URL AND ALT\n"
              u"=================\n"
              u"\n"
              u"[image: i18n][image]\n"
              u"\n"
              u"   [image: img][image]\n"
              u"\n"
              u"\n"
              u"IMAGE ON SUBSTITUTION\n"
              u"=====================\n"
              u"\n"
              u"\n"
              u"IMAGE UNDER NOTE\n"
              u"================\n"
              u"\n"
              u"Note: [image: i18n under note][image]\n"
              u"\n"
              u"     [image: img under note][image]\n"
              )
    assert result == expect


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_rubric(app):
    app.build()
    # --- rubric: regression test for pull request #190
    result = (app.outdir / 'rubric.txt').text(encoding='utf-8')
    expect = (u"I18N WITH RUBRIC"
              u"\n****************\n"
              u"\n-[ RUBRIC TITLE ]-\n"
              u"\n"
              u"\nRUBRIC IN THE BLOCK"
              u"\n===================\n"
              u"\nBLOCK\n"
              u"\n   -[ RUBRIC TITLE ]-\n")
    assert result == expect


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_docfields(app):
    app.build()
    # --- docfields
    result = (app.outdir / 'docfields.txt').text(encoding='utf-8')
    expect = (u"I18N WITH DOCFIELDS"
              u"\n*******************\n"
              u"\nclass Cls1\n"
              u"\n   Parameters:"
              u"\n      **param** -- DESCRIPTION OF PARAMETER param\n"
              u"\nclass Cls2\n"
              u"\n   Parameters:"
              u"\n      * **foo** -- DESCRIPTION OF PARAMETER foo\n"
              u"\n      * **bar** -- DESCRIPTION OF PARAMETER bar\n"
              u"\nclass Cls3(values)\n"
              u"\n   Raises:"
              u"\n      **ValueError** -- IF THE VALUES ARE OUT OF RANGE\n"
              u"\nclass Cls4(values)\n"
              u"\n   Raises:"
              u"\n      * **TypeError** -- IF THE VALUES ARE NOT VALID\n"
              u"\n      * **ValueError** -- IF THE VALUES ARE OUT OF RANGE\n"
              u"\nclass Cls5\n"
              u"\n   Returns:"
              u'\n      A NEW "Cls3" INSTANCE\n')
    assert result == expect


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_admonitions(app):
    app.build()
    # --- admonitions
    # #1206: gettext did not translate admonition directive's title
    # seealso: http://docutils.sourceforge.net/docs/ref/rst/directives.html#admonitions
    result = (app.outdir / 'admonitions.txt').text(encoding='utf-8')
    directives = (
        "attention", "caution", "danger", "error", "hint",
        "important", "note", "tip", "warning", "admonition")
    for d in directives:
        assert d.upper() + " TITLE" in result
        assert d.upper() + " BODY" in result


@sphinx_intl
@pytest.mark.sphinx('gettext')
@pytest.mark.test_params(shared_result='test_intl_gettext')
def test_gettext_toctree(app):
    app.build()
    # --- toctree
    expect = read_po(app.srcdir / 'contents.po')
    actual = read_po(app.outdir / 'contents.pot')
    for expect_msg in [m for m in expect if m.id]:
        assert expect_msg.id in [m.id for m in actual if m.id]


@sphinx_intl
@pytest.mark.sphinx('gettext')
@pytest.mark.test_params(shared_result='test_intl_gettext')
def test_gettext_table(app):
    app.build()
    # --- toctree
    expect = read_po(app.srcdir / 'table.po')
    actual = read_po(app.outdir / 'table.pot')
    for expect_msg in [m for m in expect if m.id]:
        assert expect_msg.id in [m.id for m in actual if m.id]


@sphinx_intl
@pytest.mark.sphinx('gettext')
@pytest.mark.test_params(shared_result='test_intl_gettext')
def test_gettext_definition_terms(app):
    app.build()
    # --- definition terms: regression test for #2198, #2205
    expect = read_po(app.srcdir / 'definition_terms.po')
    actual = read_po(app.outdir / 'definition_terms.pot')
    for expect_msg in [m for m in expect if m.id]:
        assert expect_msg.id in [m.id for m in actual if m.id]


@sphinx_intl
@pytest.mark.sphinx('gettext')
@pytest.mark.test_params(shared_result='test_intl_gettext')
def test_gettext_glossary_terms(app, warning):
    app.build()
    # --- glossary terms: regression test for #1090
    expect = read_po(app.srcdir / 'glossary_terms.po')
    actual = read_po(app.outdir / 'glossary_terms.pot')
    for expect_msg in [m for m in expect if m.id]:
        assert expect_msg.id in [m.id for m in actual if m.id]
    warnings = warning.getvalue().replace(os.sep, '/')
    assert 'term not in glossary' not in warnings


@sphinx_intl
@pytest.mark.sphinx('gettext')
@pytest.mark.test_params(shared_result='test_intl_gettext')
def test_gettext_glossary_term_inconsistencies(app):
    app.build()
    # --- glossary term inconsistencies: regression test for #1090
    expect = read_po(app.srcdir / 'glossary_terms_inconsistency.po')
    actual = read_po(app.outdir / 'glossary_terms_inconsistency.pot')
    for expect_msg in [m for m in expect if m.id]:
        assert expect_msg.id in [m.id for m in actual if m.id]


@sphinx_intl
@pytest.mark.sphinx('gettext')
@pytest.mark.test_params(shared_result='test_intl_gettext')
def test_gettext_literalblock(app):
    app.build()
    # --- gettext builder always ignores ``only`` directive
    expect = read_po(app.srcdir / 'literalblock.po')
    actual = read_po(app.outdir / 'literalblock.pot')
    for expect_msg in [m for m in expect if m.id]:
        if len(expect_msg.id.splitlines()) == 1:
            # compare tranlsations only labels
            assert expect_msg.id in [m.id for m in actual if m.id]
        else:
            pass  # skip code-blocks and literalblocks


@sphinx_intl
@pytest.mark.sphinx('gettext')
@pytest.mark.test_params(shared_result='test_intl_gettext')
def test_gettext_buildr_ignores_only_directive(app):
    app.build()
    # --- gettext builder always ignores ``only`` directive
    expect = read_po(app.srcdir / 'only.po')
    actual = read_po(app.outdir / 'only.pot')
    for expect_msg in [m for m in expect if m.id]:
        assert expect_msg.id in [m.id for m in actual if m.id]


@sphinx_intl
# use individual shared_result directory to avoid "incompatible doctree" error
@pytest.mark.test_params(shared_result='test_gettext_dont_rebuild_mo')
def test_gettext_dont_rebuild_mo(make_app, app_params, build_mo):
    # --- don't rebuild by .mo mtime
    def get_number_of_update_targets(app_):
        updated = app_.env.update(app_.config, app_.srcdir, app_.doctreedir)
        return len(updated)

    args, kwargs = app_params

    # phase1: build document with non-gettext builder and generate mo file in srcdir
    app0 = make_app('dummy', *args, **kwargs)
    build_mo(app0.srcdir)
    app0.build()
    assert (app0.srcdir / 'bom.mo')
    # Since it is after the build, the number of documents to be updated is 0
    assert get_number_of_update_targets(app0) == 0
    # When rewriting the timestamp of mo file, the number of documents to be
    # updated will be changed.
    mtime = (app0.srcdir / 'xx' / 'LC_MESSAGES' / 'bom.mo').stat().st_mtime
    (app0.srcdir / 'xx' / 'LC_MESSAGES' / 'bom.mo').utime((mtime + 5, mtime + 5))
    assert get_number_of_update_targets(app0) == 1

    # Because doctree for gettext builder can not be shared with other builders,
    # erase doctreedir before gettext build.
    app0.doctreedir.rmtree()

    # phase2: build document with gettext builder.
    # The mo file in the srcdir directory is retained.
    app = make_app('gettext', *args, **kwargs)
    app.build()
    # Since it is after the build, the number of documents to be updated is 0
    assert get_number_of_update_targets(app) == 0
    # Even if the timestamp of the mo file is updated, the number of documents
    # to be updated is 0. gettext builder does not rebuild because of mo update.
    (app0.srcdir / 'xx' / 'LC_MESSAGES' / 'bom.mo').utime((mtime + 10, mtime + 10))
    assert get_number_of_update_targets(app) == 0


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_html_meta(app):
    app.build()
    # --- test for meta
    result = (app.outdir / 'contents.html').text(encoding='utf-8')
    expected_expr = '<meta content="TESTDATA FOR I18N" name="description" />'
    assert expected_expr in result
    expected_expr = '<meta content="I18N, SPHINX, MARKUP" name="keywords" />'
    assert expected_expr in result


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_html_footnotes(app):
    app.build()
    # --- test for #955 cant-build-html-with-footnotes-when-using
    # expect no error by build
    (app.outdir / 'footnote.html').text(encoding='utf-8')


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_html_undefined_refs(app):
    app.build()
    # --- links to undefined reference
    result = (app.outdir / 'refs_inconsistency.html').text(encoding='utf-8')

    expected_expr = ('<a class="reference external" '
                     'href="http://www.example.com">reference</a>')
    assert len(re.findall(expected_expr, result)) == 2

    expected_expr = ('<a class="reference internal" '
                     'href="#reference">reference</a>')
    assert len(re.findall(expected_expr, result)) == 0

    expected_expr = ('<a class="reference internal" '
                     'href="#i18n-with-refs-inconsistency">I18N WITH '
                     'REFS INCONSISTENCY</a>')
    assert len(re.findall(expected_expr, result)) == 1


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_html_index_entries(app):
    app.build()
    # --- index entries: regression test for #976
    result = (app.outdir / 'genindex.html').text(encoding='utf-8')

    def wrap(tag, keyword):
        start_tag = "<%s[^>]*>" % tag
        end_tag = "</%s>" % tag
        return r"%s\s*%s\s*%s" % (start_tag, keyword, end_tag)

    def wrap_nest(parenttag, childtag, keyword):
        start_tag1 = "<%s[^>]*>" % parenttag
        start_tag2 = "<%s[^>]*>" % childtag
        return r"%s\s*%s\s*%s" % (start_tag1, keyword, start_tag2)
    expected_exprs = [
        wrap('a', 'NEWSLETTER'),
        wrap('a', 'MAILING LIST'),
        wrap('a', 'RECIPIENTS LIST'),
        wrap('a', 'FIRST SECOND'),
        wrap('a', 'SECOND THIRD'),
        wrap('a', 'THIRD, FIRST'),
        wrap_nest('li', 'ul', 'ENTRY'),
        wrap_nest('li', 'ul', 'SEE'),
        wrap('a', 'MODULE'),
        wrap('a', 'KEYWORD'),
        wrap('a', 'OPERATOR'),
        wrap('a', 'OBJECT'),
        wrap('a', 'EXCEPTION'),
        wrap('a', 'STATEMENT'),
        wrap('a', 'BUILTIN'),
    ]
    for expr in expected_exprs:
        assert_re_search(expr, result, re.M)


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_html_versionchanges(app):
    app.build()
    # --- versionchanges
    result = (app.outdir / 'versionchange.html').text(encoding='utf-8')

    def get_content(result, name):
        matched = re.search(r'<div class="%s">\n*(.*?)</div>' % name,
                            result, re.DOTALL)
        if matched:
            return matched.group(1)
        else:
            return ''

    expect1 = (
        u"""<p><span class="versionmodified">Deprecated since version 1.0: </span>"""
        u"""THIS IS THE <em>FIRST</em> PARAGRAPH OF DEPRECATED.</p>\n"""
        u"""<p>THIS IS THE <em>SECOND</em> PARAGRAPH OF DEPRECATED.</p>\n""")
    matched_content = get_content(result, "deprecated")
    assert expect1 == matched_content

    expect2 = (
        u"""<p><span class="versionmodified">New in version 1.0: </span>"""
        u"""THIS IS THE <em>FIRST</em> PARAGRAPH OF VERSIONADDED.</p>\n""")
    matched_content = get_content(result, "versionadded")
    assert expect2 == matched_content

    expect3 = (
        u"""<p><span class="versionmodified">Changed in version 1.0: </span>"""
        u"""THIS IS THE <em>FIRST</em> PARAGRAPH OF VERSIONCHANGED.</p>\n""")
    matched_content = get_content(result, "versionchanged")
    assert expect3 == matched_content


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_html_docfields(app):
    app.build()
    # --- docfields
    # expect no error by build
    (app.outdir / 'docfields.html').text(encoding='utf-8')


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_html_template(app):
    app.build()
    # --- gettext template
    result = (app.outdir / 'index.html').text(encoding='utf-8')
    assert "WELCOME" in result
    assert "SPHINX 2013.120" in result


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_html_rebuild_mo(app):
    app.build()
    # --- rebuild by .mo mtime
    app.builder.build_update()
    updated = app.env.update(app.config, app.srcdir, app.doctreedir)
    assert len(updated) == 0

    mtime = (app.srcdir / 'xx' / 'LC_MESSAGES' / 'bom.mo').stat().st_mtime
    (app.srcdir / 'xx' / 'LC_MESSAGES' / 'bom.mo').utime((mtime + 5, mtime + 5))
    updated = app.env.update(app.config, app.srcdir, app.doctreedir)
    assert len(updated) == 1


@sphinx_intl
@pytest.mark.sphinx('xml')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_xml_footnotes(app, warning):
    app.build()
    # --- footnotes: regression test for fix #955, #1176
    et = etree_parse(app.outdir / 'footnote.xml')
    secs = et.findall('section')

    para0 = secs[0].findall('paragraph')
    assert_elem(
        para0[0],
        ['I18N WITH FOOTNOTE', 'INCLUDE THIS CONTENTS',
         '2', '[ref]', '1', '100', '.'],
        ['i18n-with-footnote', 'ref'])

    footnote0 = secs[0].findall('footnote')
    assert_elem(
        footnote0[0],
        ['1', 'THIS IS A AUTO NUMBERED FOOTNOTE.'],
        None,
        ['1'])
    assert_elem(
        footnote0[1],
        ['100', 'THIS IS A NUMBERED FOOTNOTE.'],
        None,
        ['100'])
    assert_elem(
        footnote0[2],
        ['2', 'THIS IS A AUTO NUMBERED NAMED FOOTNOTE.'],
        None,
        ['named'])

    citation0 = secs[0].findall('citation')
    assert_elem(
        citation0[0],
        ['ref', 'THIS IS A NAMED FOOTNOTE.'],
        None,
        ['ref'])

    warnings = getwarning(warning)
    warning_expr = u'.*/footnote.xml:\\d*: SEVERE: Duplicate ID: ".*".\n'
    assert_not_re_search(warning_expr, warnings)


@sphinx_intl
@pytest.mark.sphinx('xml')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_xml_footnote_backlinks(app):
    app.build()
    # --- footnote backlinks: i18n test for #1058
    et = etree_parse(app.outdir / 'footnote.xml')
    secs = et.findall('section')

    para0 = secs[0].findall('paragraph')
    refs0 = para0[0].findall('footnote_reference')
    refid2id = dict([
        (r.attrib.get('refid'), r.attrib.get('ids')) for r in refs0])

    footnote0 = secs[0].findall('footnote')
    for footnote in footnote0:
        ids = footnote.attrib.get('ids')
        backrefs = footnote.attrib.get('backrefs')
        assert refid2id[ids] == backrefs


@sphinx_intl
@pytest.mark.sphinx('xml')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_xml_refs_in_python_domain(app):
    app.build()
    # --- refs in the Python domain
    et = etree_parse(app.outdir / 'refs_python_domain.xml')
    secs = et.findall('section')

    # regression test for fix #1363
    para0 = secs[0].findall('paragraph')
    assert_elem(
        para0[0],
        ['SEE THIS DECORATOR:', 'sensitive_variables()', '.'],
        ['sensitive.sensitive_variables'])


@sphinx_intl
@pytest.mark.sphinx('xml')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_xml_keep_external_links(app):
    app.build()
    # --- keep external links: regression test for #1044
    et = etree_parse(app.outdir / 'external_links.xml')
    secs = et.findall('section')

    para0 = secs[0].findall('paragraph')
    # external link check
    assert_elem(
        para0[0],
        ['EXTERNAL LINK TO', 'Python', '.'],
        ['http://python.org/index.html'])

    # internal link check
    assert_elem(
        para0[1],
        ['EXTERNAL LINKS', 'IS INTERNAL LINK.'],
        ['i18n-with-external-links'])

    # inline link check
    assert_elem(
        para0[2],
        ['INLINE LINK BY', 'THE SPHINX SITE', '.'],
        ['http://sphinx-doc.org'])

    # unnamed link check
    assert_elem(
        para0[3],
        ['UNNAMED', 'LINK', '.'],
        ['http://google.com'])

    # link target swapped translation
    para1 = secs[1].findall('paragraph')
    assert_elem(
        para1[0],
        ['LINK TO', 'external2', 'AND', 'external1', '.'],
        ['http://example.com/external2',
         'http://example.com/external1'])
    assert_elem(
        para1[1],
        ['LINK TO', 'THE PYTHON SITE', 'AND', 'THE SPHINX SITE', '.'],
        ['http://python.org', 'http://sphinx-doc.org'])

    # multiple references in the same line
    para2 = secs[2].findall('paragraph')
    assert_elem(
        para2[0],
        ['LINK TO', 'EXTERNAL LINKS', ',', 'Python', ',',
         'THE SPHINX SITE', ',', 'UNNAMED', 'AND',
         'THE PYTHON SITE', '.'],
        ['i18n-with-external-links', 'http://python.org/index.html',
         'http://sphinx-doc.org', 'http://google.com',
         'http://python.org'])


@sphinx_intl
@pytest.mark.sphinx('xml')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_xml_role_xref(app):
    app.build()
    # --- role xref: regression test for #1090, #1193
    et = etree_parse(app.outdir / 'role_xref.xml')
    sec1, sec2 = et.findall('section')

    para1, = sec1.findall('paragraph')
    assert_elem(
        para1,
        ['LINK TO', "I18N ROCK'N ROLE XREF", ',', 'CONTENTS', ',',
         'SOME NEW TERM', '.'],
        ['i18n-role-xref', 'contents',
         'glossary_terms#term-some-term'])

    para2 = sec2.findall('paragraph')
    assert_elem(
        para2[0],
        ['LINK TO', 'SOME OTHER NEW TERM', 'AND', 'SOME NEW TERM', '.'],
        ['glossary_terms#term-some-other-term',
         'glossary_terms#term-some-term'])
    assert_elem(
        para2[1],
        ['LINK TO', 'SAME TYPE LINKS', 'AND',
         "I18N ROCK'N ROLE XREF", '.'],
        ['same-type-links', 'i18n-role-xref'])
    assert_elem(
        para2[2],
        ['LINK TO', 'I18N WITH GLOSSARY TERMS', 'AND', 'CONTENTS', '.'],
        ['glossary_terms', 'contents'])
    assert_elem(
        para2[3],
        ['LINK TO', '--module', 'AND', '-m', '.'],
        ['cmdoption-module', 'cmdoption-m'])
    assert_elem(
        para2[4],
        ['LINK TO', 'env2', 'AND', 'env1', '.'],
        ['envvar-env2', 'envvar-env1'])
    assert_elem(
        para2[5],
        ['LINK TO', 'token2', 'AND', 'token1', '.'],
        [])  # TODO: how do I link token role to productionlist?
    assert_elem(
        para2[6],
        ['LINK TO', 'same-type-links', 'AND', "i18n-role-xref", '.'],
        ['same-type-links', 'i18n-role-xref'])


@sphinx_intl
@pytest.mark.sphinx('xml')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_xml_warnings(app, warning):
    app.build()
    # warnings
    warnings = getwarning(warning)
    assert 'term not in glossary' not in warnings
    assert 'undefined label' not in warnings
    assert 'unknown document' not in warnings


@sphinx_intl
@pytest.mark.sphinx('xml')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_xml_label_targets(app):
    app.build()
    # --- label targets: regression test for #1193, #1265
    et = etree_parse(app.outdir / 'label_target.xml')
    secs = et.findall('section')

    para0 = secs[0].findall('paragraph')
    assert_elem(
        para0[0],
        ['X SECTION AND LABEL', 'POINT TO', 'implicit-target', 'AND',
         'X SECTION AND LABEL', 'POINT TO', 'section-and-label', '.'],
        ['implicit-target', 'section-and-label'])

    para1 = secs[1].findall('paragraph')
    assert_elem(
        para1[0],
        ['X EXPLICIT-TARGET', 'POINT TO', 'explicit-target', 'AND',
         'X EXPLICIT-TARGET', 'POINT TO DUPLICATED ID LIKE', 'id1',
         '.'],
        ['explicit-target', 'id1'])

    para2 = secs[2].findall('paragraph')
    assert_elem(
        para2[0],
        ['X IMPLICIT SECTION NAME', 'POINT TO',
         'implicit-section-name', '.'],
        ['implicit-section-name'])

    sec2 = secs[2].findall('section')

    para2_0 = sec2[0].findall('paragraph')
    assert_elem(
        para2_0[0],
        ['`X DUPLICATED SUB SECTION`_', 'IS BROKEN LINK.'],
        [])

    para3 = secs[3].findall('paragraph')
    assert_elem(
        para3[0],
        ['X', 'bridge label',
         'IS NOT TRANSLATABLE BUT LINKED TO TRANSLATED ' +
         'SECTION TITLE.'],
        ['label-bridged-target-section'])
    assert_elem(
        para3[1],
        ['X', 'bridge label', 'POINT TO',
         'LABEL BRIDGED TARGET SECTION', 'AND', 'bridge label2',
         'POINT TO', 'SECTION AND LABEL', '. THE SECOND APPEARED',
         'bridge label2', 'POINT TO CORRECT TARGET.'],
        ['label-bridged-target-section',
         'section-and-label',
         'section-and-label'])


@sphinx_intl
@pytest.mark.sphinx('html')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_additional_targets_should_not_be_translated(app):
    app.build()
    # [literalblock.txt]
    result = (app.outdir / 'literalblock.html').text(encoding='utf-8')

    # title should be translated
    expected_expr = 'CODE-BLOCKS'
    assert_count(expected_expr, result, 2)

    # ruby code block should not be translated but be highlighted
    expected_expr = """<span class="s1">&#39;result&#39;</span>"""
    assert_count(expected_expr, result, 1)

    # C code block without lang should not be translated and *ruby* highlighted
    expected_expr = """<span class="c1">#include &lt;stdlib.h&gt;</span>"""
    assert_count(expected_expr, result, 1)

    # C code block with lang should not be translated but be *C* highlighted
    expected_expr = ("""<span class="cp">#include</span> """
                     """<span class="cpf">&lt;stdio.h&gt;</span>""")
    assert_count(expected_expr, result, 1)

    # doctest block should not be translated but be highlighted
    expected_expr = (
        """<span class="gp">&gt;&gt;&gt; </span>"""
        """<span class="kn">import</span> <span class="nn">sys</span>  """
        """<span class="c1"># sys importing</span>""")
    assert_count(expected_expr, result, 1)

    # [raw.txt]

    result = (app.outdir / 'raw.html').text(encoding='utf-8')

    # raw block should not be translated
    expected_expr = """<iframe src="http://sphinx-doc.org"></iframe></div>"""
    assert_count(expected_expr, result, 1)

    # [figure.txt]

    result = (app.outdir / 'figure.html').text(encoding='utf-8')

    # alt and src for image block should not be translated
    expected_expr = """<img alt="i18n" src="_images/i18n.png" />"""
    assert_count(expected_expr, result, 1)

    # alt and src for figure block should not be translated
    expected_expr = """<img alt="img" src="_images/img.png" />"""
    assert_count(expected_expr, result, 1)


@sphinx_intl
@pytest.mark.sphinx(
    'html',
    srcdir='test_additional_targets_should_be_translated',
    confoverrides={
        'language': 'xx', 'locale_dirs': ['.'],
        'gettext_compact': False,
        'gettext_additional_targets': [
            'index',
            'literal-block',
            'doctest-block',
            'raw',
            'image',
        ],
    }
)
def test_additional_targets_should_be_translated(app):
    app.build()
    # [literalblock.txt]
    result = (app.outdir / 'literalblock.html').text(encoding='utf-8')

    # title should be translated
    expected_expr = 'CODE-BLOCKS'
    assert_count(expected_expr, result, 2)

    # ruby code block should be translated and be highlighted
    expected_expr = """<span class="s1">&#39;RESULT&#39;</span>"""
    assert_count(expected_expr, result, 1)

    # C code block without lang should be translated and *ruby* highlighted
    expected_expr = """<span class="c1">#include &lt;STDLIB.H&gt;</span>"""
    assert_count(expected_expr, result, 1)

    # C code block with lang should be translated and be *C* highlighted
    expected_expr = ("""<span class="cp">#include</span> """
                     """<span class="cpf">&lt;STDIO.H&gt;</span>""")
    assert_count(expected_expr, result, 1)

    # doctest block should not be translated but be highlighted
    expected_expr = (
        """<span class="gp">&gt;&gt;&gt; </span>"""
        """<span class="kn">import</span> <span class="nn">sys</span>  """
        """<span class="c1"># SYS IMPORTING</span>""")
    assert_count(expected_expr, result, 1)

    # [raw.txt]

    result = (app.outdir / 'raw.html').text(encoding='utf-8')

    # raw block should be translated
    expected_expr = """<iframe src="HTTP://SPHINX-DOC.ORG"></iframe></div>"""
    assert_count(expected_expr, result, 1)

    # [figure.txt]

    result = (app.outdir / 'figure.html').text(encoding='utf-8')

    # alt and src for image block should be translated
    expected_expr = """<img alt="I18N -&gt; IMG" src="_images/img.png" />"""
    assert_count(expected_expr, result, 1)

    # alt and src for figure block should be translated
    expected_expr = """<img alt="IMG -&gt; I18N" src="_images/i18n.png" />"""
    assert_count(expected_expr, result, 1)


@sphinx_intl
@pytest.mark.sphinx('text')
@pytest.mark.test_params(shared_result='test_intl_basic')
def test_text_references(app, warning):
    app.builder.build_specific([app.srcdir / 'refs.txt'])

    warnings = warning.getvalue().replace(os.sep, '/')
    warning_expr = u'refs.txt:\\d+: ERROR: Unknown target name:'
    assert_count(warning_expr, warnings, 0)


@pytest.mark.sphinx(
    'dummy', testroot='images',
    srcdir='test_intl_images',
    confoverrides={'language': 'xx'}
)
def test_image_glob_intl(app):
    app.build()
    # index.rst
    doctree = pickle.loads((app.doctreedir / 'index.doctree').bytes())

    assert_node(doctree[0][1], nodes.image, uri='rimg.xx.png',
                candidates={'*': 'rimg.xx.png'})

    assert isinstance(doctree[0][2], nodes.figure)
    assert_node(doctree[0][2][0], nodes.image, uri='rimg.xx.png',
                candidates={'*': 'rimg.xx.png'})

    assert_node(doctree[0][3], nodes.image, uri='img.*',
                candidates={'application/pdf': 'img.pdf',
                            'image/gif': 'img.gif',
                            'image/png': 'img.png'})

    assert isinstance(doctree[0][4], nodes.figure)
    assert_node(doctree[0][4][0], nodes.image, uri='img.*',
                candidates={'application/pdf': 'img.pdf',
                            'image/gif': 'img.gif',
                            'image/png': 'img.png'})

    # subdir/index.rst
    doctree = pickle.loads((app.doctreedir / 'subdir/index.doctree').bytes())

    assert_node(doctree[0][1], nodes.image, uri='subdir/rimg.xx.png',
                candidates={'*': 'subdir/rimg.xx.png'})

    assert_node(doctree[0][2], nodes.image, uri='subdir/svgimg.*',
                candidates={'application/pdf': 'subdir/svgimg.pdf',
                            'image/svg+xml': 'subdir/svgimg.xx.svg'})

    assert isinstance(doctree[0][3], nodes.figure)
    assert_node(doctree[0][3][0], nodes.image, uri='subdir/svgimg.*',
                candidates={'application/pdf': 'subdir/svgimg.pdf',
                            'image/svg+xml': 'subdir/svgimg.xx.svg'})


@pytest.mark.sphinx(
    'dummy', testroot='images',
    srcdir='test_intl_images',
    confoverrides={
        'language': 'xx',
        'figure_language_filename': u'{root}{ext}.{language}',
    }
)
def test_image_glob_intl_using_figure_language_filename(app):
    app.build()
    # index.rst
    doctree = pickle.loads((app.doctreedir / 'index.doctree').bytes())

    assert_node(doctree[0][1], nodes.image, uri='rimg.png.xx',
                candidates={'*': 'rimg.png.xx'})

    assert isinstance(doctree[0][2], nodes.figure)
    assert_node(doctree[0][2][0], nodes.image, uri='rimg.png.xx',
                candidates={'*': 'rimg.png.xx'})

    assert_node(doctree[0][3], nodes.image, uri='img.*',
                candidates={'application/pdf': 'img.pdf',
                            'image/gif': 'img.gif',
                            'image/png': 'img.png'})

    assert isinstance(doctree[0][4], nodes.figure)
    assert_node(doctree[0][4][0], nodes.image, uri='img.*',
                candidates={'application/pdf': 'img.pdf',
                            'image/gif': 'img.gif',
                            'image/png': 'img.png'})

    # subdir/index.rst
    doctree = pickle.loads((app.doctreedir / 'subdir/index.doctree').bytes())

    assert_node(doctree[0][1], nodes.image, uri='subdir/rimg.png',
                candidates={'*': 'subdir/rimg.png'})

    assert_node(doctree[0][2], nodes.image, uri='subdir/svgimg.*',
                candidates={'application/pdf': 'subdir/svgimg.pdf',
                            'image/svg+xml': 'subdir/svgimg.svg'})

    assert isinstance(doctree[0][3], nodes.figure)
    assert_node(doctree[0][3][0], nodes.image, uri='subdir/svgimg.*',
                candidates={'application/pdf': 'subdir/svgimg.pdf',
                            'image/svg+xml': 'subdir/svgimg.svg'})


def getwarning(warnings):
    return strip_escseq(warnings.getvalue().replace(os.sep, '/'))

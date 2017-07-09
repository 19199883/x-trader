# -*- coding: utf-8 -*-
"""
    test_build_html
    ~~~~~~~~~~~~~~~

    Test the HTML builder and check output against XPath.

    :copyright: Copyright 2007-2017 by the Sphinx team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import os
import re
from itertools import cycle, chain

from six import PY3

from sphinx import __display_version__
from sphinx.util.inventory import InventoryFile
from sphinx.testing.util import remove_unicode_literals, strip_escseq
import xml.etree.cElementTree as ElementTree
from html5lib import getTreeBuilder, HTMLParser
import pytest


TREE_BUILDER = getTreeBuilder('etree', implementation=ElementTree)
HTML_PARSER = HTMLParser(TREE_BUILDER, namespaceHTMLElements=False)

ENV_WARNINGS = """\
%(root)s/autodoc_fodder.py:docstring of autodoc_fodder.MarkupError:\\d+: \
WARNING: Explicit markup ends without a blank line; unexpected unindent.
%(root)s/index.rst:\\d+: WARNING: Encoding 'utf-8-sig' used for reading included \
file u'%(root)s/wrongenc.inc' seems to be wrong, try giving an :encoding: option
%(root)s/index.rst:\\d+: WARNING: image file not readable: foo.png
%(root)s/index.rst:\\d+: WARNING: download file not readable: %(root)s/nonexisting.png
%(root)s/index.rst:\\d+: WARNING: invalid single index entry u''
%(root)s/undecodable.rst:\\d+: WARNING: undecodable source characters, replacing \
with "\\?": b?'here: >>>(\\\\|/)xbb<<<((\\\\|/)r)?'
"""

HTML_WARNINGS = ENV_WARNINGS + """\
%(root)s/index.rst:\\d+: WARNING: unknown option: &option
%(root)s/index.rst:\\d+: WARNING: citation not found: missing
%(root)s/index.rst:\\d+: WARNING: no matching candidate for image URI u'foo.\\*'
%(root)s/index.rst:\\d+: WARNING: Could not lex literal_block as "c". Highlighting skipped.
"""

if PY3:
    ENV_WARNINGS = remove_unicode_literals(ENV_WARNINGS)
    HTML_WARNINGS = remove_unicode_literals(HTML_WARNINGS)


etree_cache = {}


@pytest.fixture(scope='module')
def cached_etree_parse():
    def parse(fname):
        if fname in etree_cache:
            return etree_cache[fname]
        with (fname).open('rb') as fp:
            etree = HTML_PARSER.parse(fp)
            etree_cache.clear()
            etree_cache[fname] = etree
            return etree
    yield parse
    etree_cache.clear()


def flat_dict(d):
    return chain.from_iterable(
        [
            zip(cycle([fname]), values)
            for fname, values in d.items()
        ]
    )


def tail_check(check):
    rex = re.compile(check)

    def checker(nodes):
        for node in nodes:
            if node.tail and rex.search(node.tail):
                return True
        assert False, '%r not found in tail of any nodes %s' % (check, nodes)
    return checker


def check_xpath(etree, fname, path, check, be_found=True):
    nodes = list(etree.findall(path))
    if check is None:
        assert nodes == [], ('found any nodes matching xpath '
                             '%r in file %s' % (path, fname))
        return
    else:
        assert nodes != [], ('did not find any node matching xpath '
                             '%r in file %s' % (path, fname))
    if hasattr(check, '__call__'):
        check(nodes)
    elif not check:
        # only check for node presence
        pass
    else:
        def get_text(node):
            if node.text is not None:
                return node.text
            else:
                # Since pygments-2.1.1, empty <span> tag is inserted at top of
                # highlighting block
                if len(node) == 1 and node[0].tag == 'span' and node[0].text is None:
                    if node[0].tail is not None:
                        return node[0].tail
                return ''

        rex = re.compile(check)
        if be_found:
            if any(rex.search(get_text(node)) for node in nodes):
                return
        else:
            if all(not rex.search(get_text(node)) for node in nodes):
                return

        assert False, ('%r not found in any node matching '
                       'path %s in %s: %r' % (check, path, fname,
                                              [node.text for node in nodes]))


def check_static_entries(outdir):
    staticdir = outdir / '_static'
    assert staticdir.isdir()
    # a file from a directory entry in html_static_path
    assert (staticdir / 'README').isfile()
    # a directory from a directory entry in html_static_path
    assert (staticdir / 'subdir' / 'foo.css').isfile()
    # a file from a file entry in html_static_path
    assert (staticdir / 'templated.css').isfile()
    assert (staticdir / 'templated.css').text().splitlines()[1] == __display_version__
    # a file from _static, but matches exclude_patterns
    assert not (staticdir / 'excluded.css').exists()


def check_extra_entries(outdir):
    assert (outdir / 'robots.txt').isfile()


@pytest.mark.sphinx('html', testroot='warnings')
def test_html_warnings(app, warning):
    app.build()
    html_warnings = strip_escseq(warning.getvalue().replace(os.sep, '/'))
    html_warnings_exp = HTML_WARNINGS % {
        'root': re.escape(app.srcdir.replace(os.sep, '/'))}
    assert re.match(html_warnings_exp + '$', html_warnings), \
        'Warnings don\'t match:\n' + \
        '--- Expected (regex):\n' + html_warnings_exp + \
        '--- Got:\n' + html_warnings


@pytest.mark.sphinx('html', tags=['testtag'], confoverrides={
    'html_context.hckey_co': 'hcval_co'})
@pytest.mark.test_params(shared_result='test_build_html_output')
def test_static_output(app):
    app.build()
    check_static_entries(app.builder.outdir)
    check_extra_entries(app.builder.outdir)


@pytest.mark.parametrize("fname,expect", flat_dict({
    'images.html': [
        (".//img[@src='_images/img.png']", ''),
        (".//img[@src='_images/img1.png']", ''),
        (".//img[@src='_images/simg.png']", ''),
        (".//img[@src='_images/svgimg.svg']", ''),
        (".//a[@href='_sources/images.txt']", ''),
    ],
    'subdir/images.html': [
        (".//img[@src='../_images/img1.png']", ''),
        (".//img[@src='../_images/rimg.png']", ''),
    ],
    'subdir/includes.html': [
        (".//a[@href='../_downloads/img.png']", ''),
        (".//img[@src='../_images/img.png']", ''),
        (".//p", 'This is an include file.'),
        (".//pre/span", 'line 1'),
        (".//pre/span", 'line 2'),
    ],
    'includes.html': [
        (".//pre", u'Max Strauß'),
        (".//a[@href='_downloads/img.png']", ''),
        (".//a[@href='_downloads/img1.png']", ''),
        (".//pre/span", u'"quotes"'),
        (".//pre/span", u"'included'"),
        (".//pre/span[@class='s2']", u'üöä'),
        (".//div[@class='inc-pyobj1 highlight-text']//pre",
         r'^class Foo:\n    pass\n\s*$'),
        (".//div[@class='inc-pyobj2 highlight-text']//pre",
         r'^    def baz\(\):\n        pass\n\s*$'),
        (".//div[@class='inc-lines highlight-text']//pre",
         r'^class Foo:\n    pass\nclass Bar:\n$'),
        (".//div[@class='inc-startend highlight-text']//pre",
         u'^foo = "Including Unicode characters: üöä"\\n$'),
        (".//div[@class='inc-preappend highlight-text']//pre",
         r'(?m)^START CODE$'),
        (".//div[@class='inc-pyobj-dedent highlight-python']//span",
         r'def'),
        (".//div[@class='inc-tab3 highlight-text']//pre",
         r'-| |-'),
        (".//div[@class='inc-tab8 highlight-python']//pre/span",
         r'-|      |-'),
    ],
    'autodoc.html': [
        (".//dt[@id='test_autodoc.Class']", ''),
        (".//dt[@id='test_autodoc.function']/em", r'\*\*kwds'),
        (".//dd/p", r'Return spam\.'),
    ],
    'extapi.html': [
        (".//strong", 'from function: Foo'),
        (".//strong", 'from class: Bar'),
    ],
    'markup.html': [
        (".//title", 'set by title directive'),
        (".//p/em", 'Section author: Georg Brandl'),
        (".//p/em", 'Module author: Georg Brandl'),
        # created by the meta directive
        (".//meta[@name='author'][@content='Me']", ''),
        (".//meta[@name='keywords'][@content='docs, sphinx']", ''),
        # a label created by ``.. _label:``
        (".//div[@id='label']", ''),
        # code with standard code blocks
        (".//pre", '^some code$'),
        # an option list
        (".//span[@class='option']", '--help'),
        # admonitions
        (".//p[@class='first admonition-title']", 'My Admonition'),
        (".//p[@class='last']", 'Note text.'),
        (".//p[@class='last']", 'Warning text.'),
        # inline markup
        (".//li/strong", r'^command\\n$'),
        (".//li/strong", r'^program\\n$'),
        (".//li/em", r'^dfn\\n$'),
        (".//li/code/span[@class='pre']", r'^kbd\\n$'),
        (".//li/span", u'File \N{TRIANGULAR BULLET} Close'),
        (".//li/code/span[@class='pre']", '^a/$'),
        (".//li/code/em/span[@class='pre']", '^varpart$'),
        (".//li/code/em/span[@class='pre']", '^i$'),
        (".//a[@href='https://www.python.org/dev/peps/pep-0008']"
         "[@class='pep reference external']/strong", 'PEP 8'),
        (".//a[@href='https://www.python.org/dev/peps/pep-0008']"
         "[@class='pep reference external']/strong",
         'Python Enhancement Proposal #8'),
        (".//a[@href='https://tools.ietf.org/html/rfc1.html']"
         "[@class='rfc reference external']/strong", 'RFC 1'),
        (".//a[@href='https://tools.ietf.org/html/rfc1.html']"
         "[@class='rfc reference external']/strong", 'Request for Comments #1'),
        (".//a[@href='objects.html#envvar-HOME']"
         "[@class='reference internal']/code/span[@class='pre']", 'HOME'),
        (".//a[@href='#with']"
         "[@class='reference internal']/code/span[@class='pre']", '^with$'),
        (".//a[@href='#grammar-token-try_stmt']"
         "[@class='reference internal']/code/span", '^statement$'),
        (".//a[@href='#some-label'][@class='reference internal']/span", '^here$'),
        (".//a[@href='#some-label'][@class='reference internal']/span", '^there$'),
        (".//a[@href='subdir/includes.html']"
         "[@class='reference internal']/span", 'Including in subdir'),
        (".//a[@href='objects.html#cmdoption-python-c']"
         "[@class='reference internal']/code/span[@class='pre']", '-c'),
        # abbreviations
        (".//abbr[@title='abbreviation']", '^abbr$'),
        # version stuff
        (".//div[@class='versionadded']/p/span", 'New in version 0.6: '),
        (".//div[@class='versionadded']/p/span",
         tail_check('First paragraph of versionadded')),
        (".//div[@class='versionchanged']/p/span",
         tail_check('First paragraph of versionchanged')),
        (".//div[@class='versionchanged']/p",
         'Second paragraph of versionchanged'),
        # footnote reference
        (".//a[@class='footnote-reference']", r'\[1\]'),
        # created by reference lookup
        (".//a[@href='contents.html#ref1']", ''),
        # ``seealso`` directive
        (".//div/p[@class='first admonition-title']", 'See also'),
        # a ``hlist`` directive
        (".//table[@class='hlist']/tbody/tr/td/ul/li", '^This$'),
        # a ``centered`` directive
        (".//p[@class='centered']/strong", 'LICENSE'),
        # a glossary
        (".//dl/dt[@id='term-boson']", 'boson'),
        # a production list
        (".//pre/strong", 'try_stmt'),
        (".//pre/a[@href='#grammar-token-try1_stmt']/code/span", 'try1_stmt'),
        # tests for ``only`` directive
        (".//p", 'A global substitution.'),
        (".//p", 'In HTML.'),
        (".//p", 'In both.'),
        (".//p", 'Always present'),
        # tests for ``any`` role
        (".//a[@href='#with']/span", 'headings'),
        (".//a[@href='objects.html#func_without_body']/code/span", 'objects'),
        # tests for smartypants
        (".//li", u'Smart “quotes” in English ‘text’.'),
        (".//li", u'Smart — long and – short dashes.'),
        (".//li", u'Ellipsis…'),
        (".//li//code//span[@class='pre']", 'foo--"bar"...'),
        (".//p", u'Этот «абзац» должен использовать „русские“ кавычки.'),
        (".//p", u'Il dit : « C’est “super” ! »'),
    ],
    'objects.html': [
        (".//dt[@id='mod.Cls.meth1']", ''),
        (".//dt[@id='errmod.Error']", ''),
        (".//dt/code", r'long\(parameter,\s* list\)'),
        (".//dt/code", 'another one'),
        (".//a[@href='#mod.Cls'][@class='reference internal']", ''),
        (".//dl[@class='userdesc']", ''),
        (".//dt[@id='userdesc-myobj']", ''),
        (".//a[@href='#userdesc-myobj'][@class='reference internal']", ''),
        # docfields
        (".//a[@class='reference internal'][@href='#TimeInt']/em", 'TimeInt'),
        (".//a[@class='reference internal'][@href='#Time']", 'Time'),
        (".//a[@class='reference internal'][@href='#errmod.Error']/strong", 'Error'),
        # C references
        (".//span[@class='pre']", 'CFunction()'),
        (".//a[@href='#c.Sphinx_DoSomething']", ''),
        (".//a[@href='#c.SphinxStruct.member']", ''),
        (".//a[@href='#c.SPHINX_USE_PYTHON']", ''),
        (".//a[@href='#c.SphinxType']", ''),
        (".//a[@href='#c.sphinx_global']", ''),
        # test global TOC created by toctree()
        (".//ul[@class='current']/li[@class='toctree-l1 current']/a[@href='#']",
         'Testing object descriptions'),
        (".//li[@class='toctree-l1']/a[@href='markup.html']",
         'Testing various markup'),
        # test unknown field names
        (".//th[@class='field-name']", 'Field_name:'),
        (".//th[@class='field-name']", 'Field_name all lower:'),
        (".//th[@class='field-name']", 'FIELD_NAME:'),
        (".//th[@class='field-name']", 'FIELD_NAME ALL CAPS:'),
        (".//th[@class='field-name']", 'Field_Name:'),
        (".//th[@class='field-name']", 'Field_Name All Word Caps:'),
        (".//th[@class='field-name']", 'Field_name:'),
        (".//th[@class='field-name']", 'Field_name First word cap:'),
        (".//th[@class='field-name']", 'FIELd_name:'),
        (".//th[@class='field-name']", 'FIELd_name PARTial caps:'),
        # custom sidebar
        (".//h4", 'Custom sidebar'),
        # docfields
        (".//td[@class='field-body']/strong", '^moo$'),
        (".//td[@class='field-body']/strong", tail_check(r'\(Moo\) .* Moo')),
        (".//td[@class='field-body']/ul/li/strong", '^hour$'),
        (".//td[@class='field-body']/ul/li/em", '^DuplicateType$'),
        (".//td[@class='field-body']/ul/li/em", tail_check(r'.* Some parameter')),
        # others
        (".//a[@class='reference internal'][@href='#cmdoption-perl-arg-p']/code/span",
         'perl'),
        (".//a[@class='reference internal'][@href='#cmdoption-perl-arg-p']/code/span",
         '\\+p'),
        (".//a[@class='reference internal'][@href='#cmdoption-perl-objc']/code/span",
         '--ObjC\\+\\+'),
        (".//a[@class='reference internal'][@href='#cmdoption-perl-plugin-option']/code/span",
         '--plugin.option'),
        (".//a[@class='reference internal'][@href='#cmdoption-perl-arg-create-auth-token']"
         "/code/span",
         'create-auth-token'),
        (".//a[@class='reference internal'][@href='#cmdoption-perl-arg-arg']/code/span",
         'arg'),
        (".//a[@class='reference internal'][@href='#cmdoption-hg-arg-commit']/code/span",
         'hg'),
        (".//a[@class='reference internal'][@href='#cmdoption-hg-arg-commit']/code/span",
         'commit'),
        (".//a[@class='reference internal'][@href='#cmdoption-git-commit-p']/code/span",
         'git'),
        (".//a[@class='reference internal'][@href='#cmdoption-git-commit-p']/code/span",
         'commit'),
        (".//a[@class='reference internal'][@href='#cmdoption-git-commit-p']/code/span",
         '-p'),
    ],
    'contents.html': [
        (".//meta[@name='hc'][@content='hcval']", ''),
        (".//meta[@name='hc_co'][@content='hcval_co']", ''),
        (".//meta[@name='testopt'][@content='testoverride']", ''),
        (".//td[@class='label']", r'\[Ref1\]'),
        (".//td[@class='label']", ''),
        (".//li[@class='toctree-l1']/a", 'Testing various markup'),
        (".//li[@class='toctree-l2']/a", 'Inline markup'),
        (".//title", 'Sphinx <Tests>'),
        (".//div[@class='footer']", 'Georg Brandl & Team'),
        (".//a[@href='http://python.org/']"
         "[@class='reference external']", ''),
        (".//li/a[@href='genindex.html']/span", 'Index'),
        (".//li/a[@href='py-modindex.html']/span", 'Module Index'),
        (".//li/a[@href='search.html']/span", 'Search Page'),
        # custom sidebar only for contents
        (".//h4", 'Contents sidebar'),
        # custom JavaScript
        (".//script[@src='file://moo.js']", ''),
        # URL in contents
        (".//a[@class='reference external'][@href='http://sphinx-doc.org/']",
         'http://sphinx-doc.org/'),
        (".//a[@class='reference external'][@href='http://sphinx-doc.org/latest/']",
         'Latest reference'),
        # Indirect hyperlink targets across files
        (".//a[@href='markup.html#some-label'][@class='reference internal']/span",
         '^indirect hyperref$'),
    ],
    'bom.html': [
        (".//title", " File with UTF-8 BOM"),
    ],
    'extensions.html': [
        (".//a[@href='http://python.org/dev/']", "http://python.org/dev/"),
        (".//a[@href='http://bugs.python.org/issue1000']", "issue 1000"),
        (".//a[@href='http://bugs.python.org/issue1042']", "explicit caption"),
    ],
    '_static/statictmpl.html': [
        (".//project", 'Sphinx <Tests>'),
    ],
    'genindex.html': [
        # index entries
        (".//a/strong", "Main"),
        (".//a/strong", "[1]"),
        (".//a/strong", "Other"),
        (".//a", "entry"),
        (".//li/a", "double"),
    ],
    'footnote.html': [
        (".//a[@class='footnote-reference'][@href='#id8'][@id='id1']", r"\[1\]"),
        (".//a[@class='footnote-reference'][@href='#id9'][@id='id2']", r"\[2\]"),
        (".//a[@class='footnote-reference'][@href='#foo'][@id='id3']", r"\[3\]"),
        (".//a[@class='reference internal'][@href='#bar'][@id='id4']", r"\[bar\]"),
        (".//a[@class='footnote-reference'][@href='#id10'][@id='id5']", r"\[4\]"),
        (".//a[@class='footnote-reference'][@href='#id11'][@id='id6']", r"\[5\]"),
        (".//a[@class='fn-backref'][@href='#id1']", r"\[1\]"),
        (".//a[@class='fn-backref'][@href='#id2']", r"\[2\]"),
        (".//a[@class='fn-backref'][@href='#id3']", r"\[3\]"),
        (".//a[@class='fn-backref'][@href='#id4']", r"\[bar\]"),
        (".//a[@class='fn-backref'][@href='#id5']", r"\[4\]"),
        (".//a[@class='fn-backref'][@href='#id6']", r"\[5\]"),
        (".//a[@class='fn-backref'][@href='#id7']", r"\[6\]"),
    ],
    'otherext.html': [
        (".//h1", "Generated section"),
        (".//a[@href='_sources/otherext.foo.txt']", ''),
    ]
}))
@pytest.mark.sphinx('html', tags=['testtag'], confoverrides={
    'html_context.hckey_co': 'hcval_co'})
@pytest.mark.test_params(shared_result='test_build_html_output')
def test_html_output(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.sphinx('html', testroot='build-html-translator')
def test_html_translator(app):
    app.build()
    assert app.builder.docwriter.visitor.depart_with_node == 10


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//li[@class='toctree-l3']/a", '1.1.1. Foo A1', True),
        (".//li[@class='toctree-l3']/a", '1.2.1. Foo B1', True),
        (".//li[@class='toctree-l3']/a", '2.1.1. Bar A1', False),
        (".//li[@class='toctree-l3']/a", '2.2.1. Bar B1', False),
    ],
    'foo.html': [
        (".//h1", '1. Foo', True),
        (".//h2", '1.1. Foo A', True),
        (".//h3", '1.1.1. Foo A1', True),
        (".//h2", '1.2. Foo B', True),
        (".//h3", '1.2.1. Foo B1', True),
        (".//div[@class='sphinxsidebarwrapper']//li/a", '1.1. Foo A', True),
        (".//div[@class='sphinxsidebarwrapper']//li/a", '1.1.1. Foo A1', True),
        (".//div[@class='sphinxsidebarwrapper']//li/a", '1.2. Foo B', True),
        (".//div[@class='sphinxsidebarwrapper']//li/a", '1.2.1. Foo B1', True),
    ],
    'bar.html': [
        (".//h1", '2. Bar', True),
        (".//h2", '2.1. Bar A', True),
        (".//h2", '2.2. Bar B', True),
        (".//h3", '2.2.1. Bar B1', True),
        (".//div[@class='sphinxsidebarwrapper']//li/a", '2. Bar', True),
        (".//div[@class='sphinxsidebarwrapper']//li/a", '2.1. Bar A', True),
        (".//div[@class='sphinxsidebarwrapper']//li/a", '2.2. Bar B', True),
        (".//div[@class='sphinxsidebarwrapper']//li/a", '2.2.1. Bar B1', False),
    ],
    'baz.html': [
        (".//h1", '2.1.1. Baz A', True),
    ],
}))
@pytest.mark.sphinx('html', testroot='tocdepth')
@pytest.mark.test_params(shared_result='test_build_html_tocdepth')
def test_tocdepth(app, cached_etree_parse, fname, expect):
    app.build()
    # issue #1251
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//li[@class='toctree-l3']/a", '1.1.1. Foo A1', True),
        (".//li[@class='toctree-l3']/a", '1.2.1. Foo B1', True),
        (".//li[@class='toctree-l3']/a", '2.1.1. Bar A1', False),
        (".//li[@class='toctree-l3']/a", '2.2.1. Bar B1', False),

        # index.rst
        (".//h1", 'test-tocdepth', True),

        # foo.rst
        (".//h2", '1. Foo', True),
        (".//h3", '1.1. Foo A', True),
        (".//h4", '1.1.1. Foo A1', True),
        (".//h3", '1.2. Foo B', True),
        (".//h4", '1.2.1. Foo B1', True),

        # bar.rst
        (".//h2", '2. Bar', True),
        (".//h3", '2.1. Bar A', True),
        (".//h3", '2.2. Bar B', True),
        (".//h4", '2.2.1. Bar B1', True),

        # baz.rst
        (".//h4", '2.1.1. Baz A', True),
    ],
}))
@pytest.mark.sphinx('singlehtml', testroot='tocdepth')
@pytest.mark.test_params(shared_result='test_build_html_tocdepth')
def test_tocdepth_singlehtml(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.sphinx('html', testroot='numfig')
@pytest.mark.test_params(shared_result='test_build_html_numfig')
def test_numfig_disabled_warn(app, warning):
    app.build()
    warnings = warning.getvalue()
    assert 'index.rst:47: WARNING: numfig is disabled. :numref: is ignored.' in warnings
    assert 'index.rst:55: WARNING: no number is assigned for section: index' not in warnings
    assert 'index.rst:56: WARNING: invalid numfig_format: invalid' not in warnings
    assert 'index.rst:57: WARNING: invalid numfig_format: Fig %s %s' not in warnings


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", None, True),
        (".//table/caption/span[@class='caption-number']", None, True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", None, True),
        (".//li/code/span", '^fig1$', True),
        (".//li/code/span", '^Figure%s$', True),
        (".//li/code/span", '^table-1$', True),
        (".//li/code/span", '^Table:%s$', True),
        (".//li/code/span", '^CODE_1$', True),
        (".//li/code/span", '^Code-%s$', True),
        (".//li/code/span", '^foo$', True),
        (".//li/code/span", '^bar_a$', True),
        (".//li/code/span", '^Fig.{number}$', True),
        (".//li/code/span", '^Sect.{number}$', True),
    ],
    'foo.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", None, True),
        (".//table/caption/span[@class='caption-number']", None, True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", None, True),
    ],
    'bar.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", None, True),
        (".//table/caption/span[@class='caption-number']", None, True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", None, True),
    ],
    'baz.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", None, True),
        (".//table/caption/span[@class='caption-number']", None, True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", None, True),
    ],
}))
@pytest.mark.sphinx('html', testroot='numfig')
@pytest.mark.test_params(shared_result='test_build_html_numfig')
def test_numfig_disabled(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.sphinx(
    'html', testroot='numfig',
    srcdir='test_numfig_without_numbered_toctree_warn',
    confoverrides={'numfig': True})
def test_numfig_without_numbered_toctree_warn(app, warning):
    app.build()
    # remove :numbered: option
    index = (app.srcdir / 'index.rst').text()
    index = re.sub(':numbered:.*', '', index)
    (app.srcdir / 'index.rst').write_text(index, encoding='utf-8')
    app.builder.build_all()

    warnings = warning.getvalue()
    assert 'index.rst:47: WARNING: numfig is disabled. :numref: is ignored.' not in warnings
    assert 'index.rst:55: WARNING: no number is assigned for section: index' in warnings
    assert 'index.rst:56: WARNING: invalid numfig_format: invalid' in warnings
    assert 'index.rst:57: WARNING: invalid numfig_format: Fig %s %s' in warnings


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 9 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 10 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 9 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 10 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 9 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 10 $', True),
        (".//li/a/span", '^Fig. 9$', True),
        (".//li/a/span", '^Figure6$', True),
        (".//li/a/span", '^Table 9$', True),
        (".//li/a/span", '^Table:6$', True),
        (".//li/a/span", '^Listing 9$', True),
        (".//li/a/span", '^Code-6$', True),
        (".//li/code/span", '^foo$', True),
        (".//li/code/span", '^bar_a$', True),
        (".//li/a/span", '^Fig.9 should be Fig.1$', True),
        (".//li/code/span", '^Sect.{number}$', True),
    ],
    'foo.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 3 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 4 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 3 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 4 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 3 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 4 $', True),
    ],
    'bar.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 5 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 7 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 8 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 5 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 7 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 8 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 5 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 7 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 8 $', True),
    ],
    'baz.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 6 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 6 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 6 $', True),
    ],
}))
@pytest.mark.sphinx(
    'html', testroot='numfig',
    srcdir='test_numfig_without_numbered_toctree',
    confoverrides={'numfig': True})
def test_numfig_without_numbered_toctree(app, cached_etree_parse, fname, expect):
    # remove :numbered: option
    index = (app.srcdir / 'index.rst').text()
    index = re.sub(':numbered:.*', '', index)
    (app.srcdir / 'index.rst').write_text(index, encoding='utf-8')

    if not app.outdir.listdir():
        app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.sphinx('html', testroot='numfig', confoverrides={'numfig': True})
@pytest.mark.test_params(shared_result='test_build_html_numfig_on')
def test_numfig_with_numbered_toctree_warn(app, warning):
    app.build()
    warnings = warning.getvalue()
    assert 'index.rst:47: WARNING: numfig is disabled. :numref: is ignored.' not in warnings
    assert 'index.rst:55: WARNING: no number is assigned for section: index' in warnings
    assert 'index.rst:56: WARNING: invalid numfig_format: invalid' in warnings
    assert 'index.rst:57: WARNING: invalid numfig_format: Fig %s %s' in warnings


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2 $', True),
        (".//li/a/span", '^Fig. 1$', True),
        (".//li/a/span", '^Figure2.2$', True),
        (".//li/a/span", '^Table 1$', True),
        (".//li/a/span", '^Table:2.2$', True),
        (".//li/a/span", '^Listing 1$', True),
        (".//li/a/span", '^Code-2.2$', True),
        (".//li/a/span", '^Section.1$', True),
        (".//li/a/span", '^Section.2.1$', True),
        (".//li/a/span", '^Fig.1 should be Fig.1$', True),
        (".//li/a/span", '^Sect.1 Foo$', True),
    ],
    'foo.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.2 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.3 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.4 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.3 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.4 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.3 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.4 $', True),
    ],
    'bar.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.3 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.4 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.3 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.4 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.3 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.4 $', True),
    ],
    'baz.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.2 $', True),
    ],
}))
@pytest.mark.sphinx('html', testroot='numfig', confoverrides={'numfig': True})
@pytest.mark.test_params(shared_result='test_build_html_numfig_on')
def test_numfig_with_numbered_toctree(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.sphinx('html', testroot='numfig', confoverrides={
    'numfig': True,
    'numfig_format': {'figure': 'Figure:%s',
                      'table': 'Tab_%s',
                      'code-block': 'Code-%s',
                      'section': 'SECTION-%s'}})
@pytest.mark.test_params(shared_result='test_build_html_numfig_format_warn')
def test_numfig_with_prefix_warn(app, warning):
    app.build()
    warnings = warning.getvalue()
    assert 'index.rst:47: WARNING: numfig is disabled. :numref: is ignored.' not in warnings
    assert 'index.rst:55: WARNING: no number is assigned for section: index' in warnings
    assert 'index.rst:56: WARNING: invalid numfig_format: invalid' in warnings
    assert 'index.rst:57: WARNING: invalid numfig_format: Fig %s %s' in warnings


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-2 $', True),
        (".//li/a/span", '^Figure:1$', True),
        (".//li/a/span", '^Figure2.2$', True),
        (".//li/a/span", '^Tab_1$', True),
        (".//li/a/span", '^Table:2.2$', True),
        (".//li/a/span", '^Code-1$', True),
        (".//li/a/span", '^Code-2.2$', True),
        (".//li/a/span", '^SECTION-1$', True),
        (".//li/a/span", '^SECTION-2.1$', True),
        (".//li/a/span", '^Fig.1 should be Fig.1$', True),
        (".//li/a/span", '^Sect.1 Foo$', True),
    ],
    'foo.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:1.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:1.2 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:1.3 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:1.4 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_1.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_1.2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_1.3 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_1.4 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-1.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-1.2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-1.3 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-1.4 $', True),
    ],
    'bar.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:2.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:2.3 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:2.4 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_2.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_2.3 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_2.4 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-2.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-2.3 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-2.4 $', True),
    ],
    'baz.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Figure:2.2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Tab_2.2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Code-2.2 $', True),
    ],
}))
@pytest.mark.sphinx('html', testroot='numfig', confoverrides={
    'numfig': True,
    'numfig_format': {'figure': 'Figure:%s',
                      'table': 'Tab_%s',
                      'code-block': 'Code-%s',
                      'section': 'SECTION-%s'}})
@pytest.mark.test_params(shared_result='test_build_html_numfig_format_warn')
def test_numfig_with_prefix(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.sphinx('html', testroot='numfig', confoverrides={
    'numfig': True, 'numfig_secnum_depth': 2})
@pytest.mark.test_params(shared_result='test_build_html_numfig_depth_2')
def test_numfig_with_secnum_depth_warn(app, warning):
    app.build()
    warnings = warning.getvalue()
    assert 'index.rst:47: WARNING: numfig is disabled. :numref: is ignored.' not in warnings
    assert 'index.rst:55: WARNING: no number is assigned for section: index' in warnings
    assert 'index.rst:56: WARNING: invalid numfig_format: invalid' in warnings
    assert 'index.rst:57: WARNING: invalid numfig_format: Fig %s %s' in warnings


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2 $', True),
        (".//li/a/span", '^Fig. 1$', True),
        (".//li/a/span", '^Figure2.1.2$', True),
        (".//li/a/span", '^Table 1$', True),
        (".//li/a/span", '^Table:2.1.2$', True),
        (".//li/a/span", '^Listing 1$', True),
        (".//li/a/span", '^Code-2.1.2$', True),
        (".//li/a/span", '^Section.1$', True),
        (".//li/a/span", '^Section.2.1$', True),
        (".//li/a/span", '^Fig.1 should be Fig.1$', True),
        (".//li/a/span", '^Sect.1 Foo$', True),
    ],
    'foo.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.1.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.1.2 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.2.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.1.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.1.2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.2.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.1.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.1.2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.2.1 $', True),
    ],
    'bar.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.1.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.1.3 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.2.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.1.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.1.3 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.2.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.1.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.1.3 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.2.1 $', True),
    ],
    'baz.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.1.2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.1.2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.1.2 $', True),
    ],
}))
@pytest.mark.sphinx('html', testroot='numfig', confoverrides={
    'numfig': True, 'numfig_secnum_depth': 2})
@pytest.mark.test_params(shared_result='test_build_html_numfig_depth_2')
def test_numfig_with_secnum_depth(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2 $', True),
        (".//li/a/span", '^Fig. 1$', True),
        (".//li/a/span", '^Figure2.2$', True),
        (".//li/a/span", '^Table 1$', True),
        (".//li/a/span", '^Table:2.2$', True),
        (".//li/a/span", '^Listing 1$', True),
        (".//li/a/span", '^Code-2.2$', True),
        (".//li/a/span", '^Section.1$', True),
        (".//li/a/span", '^Section.2.1$', True),
        (".//li/a/span", '^Fig.1 should be Fig.1$', True),
        (".//li/a/span", '^Sect.1 Foo$', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.2 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.3 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 1.4 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.3 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 1.4 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.3 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 1.4 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.1 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.3 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.4 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.1 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.3 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.4 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.1 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.3 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.4 $', True),
        (".//div[@class='figure']/p[@class='caption']/"
         "span[@class='caption-number']", '^Fig. 2.2 $', True),
        (".//table/caption/span[@class='caption-number']",
         '^Table 2.2 $', True),
        (".//div[@class='code-block-caption']/"
         "span[@class='caption-number']", '^Listing 2.2 $', True),
    ],
}))
@pytest.mark.sphinx('singlehtml', testroot='numfig', confoverrides={
    'numfig': True})
@pytest.mark.test_params(shared_result='test_build_html_numfig_on')
def test_numfig_with_singlehtml(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//div[@class='figure']/p[@class='caption']/span[@class='caption-number']",
         "Fig. 1", True),
        (".//div[@class='figure']/p[@class='caption']/span[@class='caption-number']",
         "Fig. 2", True),
        (".//div[@class='figure']/p[@class='caption']/span[@class='caption-number']",
         "Fig. 3", True),
        (".//div//span[@class='caption-number']", "No.1 ", True),
        (".//div//span[@class='caption-number']", "No.2 ", True),
        (".//li/a/span", 'Fig. 1', True),
        (".//li/a/span", 'Fig. 2', True),
        (".//li/a/span", 'Fig. 3', True),
        (".//li/a/span", 'No.1', True),
        (".//li/a/span", 'No.2', True),
    ],
}))
@pytest.mark.sphinx(
    'html', testroot='add_enumerable_node',
    srcdir='test_enumerable_node',
)
def test_enumerable_node(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.sphinx('html', testroot='html_assets')
def test_html_assets(app):
    app.builder.build_all()

    # html_static_path
    assert not (app.outdir / '_static' / '.htaccess').exists()
    assert not (app.outdir / '_static' / '.htpasswd').exists()
    assert (app.outdir / '_static' / 'API.html').exists()
    assert (app.outdir / '_static' / 'API.html').text() == 'Sphinx-1.4.4'
    assert (app.outdir / '_static' / 'css/style.css').exists()
    assert (app.outdir / '_static' / 'rimg.png').exists()
    assert not (app.outdir / '_static' / '_build/index.html').exists()
    assert (app.outdir / '_static' / 'background.png').exists()
    assert not (app.outdir / '_static' / 'subdir' / '.htaccess').exists()
    assert not (app.outdir / '_static' / 'subdir' / '.htpasswd').exists()

    # html_extra_path
    assert (app.outdir / '.htaccess').exists()
    assert not (app.outdir / '.htpasswd').exists()
    assert (app.outdir / 'API.html_t').exists()
    assert (app.outdir / 'css/style.css').exists()
    assert (app.outdir / 'rimg.png').exists()
    assert not (app.outdir / '_build/index.html').exists()
    assert (app.outdir / 'background.png').exists()
    assert (app.outdir / 'subdir' / '.htaccess').exists()
    assert not (app.outdir / 'subdir' / '.htpasswd').exists()


@pytest.mark.sphinx('html', confoverrides={'html_sourcelink_suffix': ''})
def test_html_sourcelink_suffix(app):
    app.builder.build_all()
    content_otherext = (app.outdir / 'otherext.html').text()
    content_images = (app.outdir / 'images.html').text()

    assert '<a href="_sources/otherext.foo"' in content_otherext
    assert '<a href="_sources/images.txt"' in content_images
    assert (app.outdir / '_sources' / 'otherext.foo').exists()
    assert (app.outdir / '_sources' / 'images.txt').exists()


@pytest.mark.sphinx('html', testroot='html_entity')
def test_html_entity(app):
    app.builder.build_all()
    valid_entities = {'amp', 'lt', 'gt', 'quot', 'apos'}
    content = (app.outdir / 'index.html').text()
    for entity in re.findall(r'&([a-z]+);', content, re.M):
        assert entity not in valid_entities


@pytest.mark.sphinx('html', testroot='basic')
def test_html_inventory(app):
    app.builder.build_all()
    with open(app.outdir / 'objects.inv', 'rb') as f:
        invdata = InventoryFile.load(f, 'http://example.com', os.path.join)
    assert set(invdata.keys()) == {'std:label', 'std:doc'}
    assert set(invdata['std:label'].keys()) == {'modindex', 'genindex', 'search'}
    assert invdata['std:label']['modindex'] == ('Python',
                                                '',
                                                'http://example.com/py-modindex.html',
                                                'Module Index')
    assert invdata['std:label']['genindex'] == ('Python',
                                                '',
                                                'http://example.com/genindex.html',
                                                'Index')
    assert invdata['std:label']['search'] == ('Python',
                                              '',
                                              'http://example.com/search.html',
                                              'Search Page')
    assert set(invdata['std:doc'].keys()) == {'index'}
    assert invdata['std:doc']['index'] == ('Python',
                                           '',
                                           'http://example.com/index.html',
                                           'The basic Sphinx documentation for testing')


@pytest.mark.sphinx('html', testroot='directives-raw')
def test_html_raw_directive(app, status, warning):
    app.builder.build_all()
    result = (app.outdir / 'index.html').text(encoding='utf8')

    # standard case
    assert 'standalone raw directive (HTML)' in result
    assert 'standalone raw directive (LaTeX)' not in result

    # with substitution
    assert '<p>HTML: abc def ghi</p>' in result
    assert '<p>LaTeX: abc  ghi</p>' in result


@pytest.mark.parametrize("fname,expect", flat_dict({
    'index.html': [
        (".//link[@href='_static/persistent.css']"
         "[@rel='stylesheet']", '', True),
        (".//link[@href='_static/default.css']"
         "[@rel='stylesheet']"
         "[@title='Default']", '', True),
        (".//link[@href='_static/alternate1.css']"
         "[@rel='alternate stylesheet']"
         "[@title='Alternate']", '', True),
        (".//link[@href='_static/alternate2.css']"
         "[@rel='alternate stylesheet']", '', True),
        (".//link[@href='_static/more_persistent.css']"
         "[@rel='stylesheet']", '', True),
        (".//link[@href='_static/more_default.css']"
         "[@rel='stylesheet']"
         "[@title='Default']", '', True),
        (".//link[@href='_static/more_alternate1.css']"
         "[@rel='alternate stylesheet']"
         "[@title='Alternate']", '', True),
        (".//link[@href='_static/more_alternate2.css']"
         "[@rel='alternate stylesheet']", '', True),
    ],
}))
@pytest.mark.sphinx('html', testroot='stylesheets')
def test_alternate_stylesheets(app, cached_etree_parse, fname, expect):
    app.build()
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)


@pytest.mark.sphinx('html', testroot='images')
def test_html_remote_images(app, status, warning):
    app.builder.build_all()

    result = (app.outdir / 'index.html').text(encoding='utf8')
    assert ('<img alt="https://www.python.org/static/img/python-logo.png" '
            'src="https://www.python.org/static/img/python-logo.png" />' in result)
    assert not (app.outdir / 'python-logo.png').exists()

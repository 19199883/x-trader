# -*- coding: utf-8 -*-
"""
    test_build_html5
    ~~~~~~~~~~~~~~~~

    Test the HTML5 writer and check output against XPath.

    This code is digest to reduce test running time.
    Complete test code is here:

    https://github.com/sphinx-doc/sphinx/pull/2805/files

    :copyright: Copyright 2007-2017 by the Sphinx team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import xml.etree.cElementTree as ElementTree

import pytest
from html5lib import getTreeBuilder, HTMLParser

from sphinx.util.docutils import is_html5_writer_available

from sphinx.testing.util import skip_unless
from test_build_html import flat_dict, tail_check, check_xpath

TREE_BUILDER = getTreeBuilder('etree', implementation=ElementTree)
HTML_PARSER = HTMLParser(TREE_BUILDER, namespaceHTMLElements=False)


etree_cache = {}


@skip_unless(is_html5_writer_available())
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


@skip_unless(is_html5_writer_available())
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
        (".//p[@class='admonition-title']", 'My Admonition'),
        (".//div[@class='admonition note']/p", 'Note text.'),
        (".//div[@class='admonition warning']/p", 'Warning text.'),
        # inline markup
        (".//li/p/strong", r'^command\\n$'),
        (".//li/p/strong", r'^program\\n$'),
        (".//li/p/em", r'^dfn\\n$'),
        (".//li/p/code/span[@class='pre']", r'^kbd\\n$'),
        (".//li/p/span", u'File \N{TRIANGULAR BULLET} Close'),
        (".//li/p/code/span[@class='pre']", '^a/$'),
        (".//li/p/code/em/span[@class='pre']", '^varpart$'),
        (".//li/p/code/em/span[@class='pre']", '^i$'),
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
        (".//a[@class='footnote-reference brackets']", r'1'),
        # created by reference lookup
        (".//a[@href='contents.html#ref1']", ''),
        # ``seealso`` directive
        (".//div/p[@class='admonition-title']", 'See also'),
        # a ``hlist`` directive
        (".//table[@class='hlist']/tbody/tr/td/ul/li/p", '^This$'),
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
        (".//dt[@class='field-odd']", 'Field_name'),
        (".//dt[@class='field-even']", 'Field_name all lower'),
        (".//dt[@class='field-odd']", 'FIELD_NAME'),
        (".//dt[@class='field-even']", 'FIELD_NAME ALL CAPS'),
        (".//dt[@class='field-odd']", 'Field_Name'),
        (".//dt[@class='field-even']", 'Field_Name All Word Caps'),
        (".//dt[@class='field-odd']", 'Field_name'),
        (".//dt[@class='field-even']", 'Field_name First word cap'),
        (".//dt[@class='field-odd']", 'FIELd_name'),
        (".//dt[@class='field-even']", 'FIELd_name PARTial caps'),
        # custom sidebar
        (".//h4", 'Custom sidebar'),
        # docfields
        (".//dd[@class='field-odd']/p/strong", '^moo$'),
        (".//dd[@class='field-odd']/p/strong", tail_check(r'\(Moo\) .* Moo')),
        (".//dd[@class='field-odd']/ul/li/p/strong", '^hour$'),
        (".//dd[@class='field-odd']/ul/li/p/em", '^DuplicateType$'),
        (".//dd[@class='field-odd']/ul/li/p/em", tail_check(r'.* Some parameter')),
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
        (".//dt[@class='label']/span[@class='brackets']", r'Ref1'),
        (".//dt[@class='label']", ''),
        (".//li[@class='toctree-l1']/a", 'Testing various markup'),
        (".//li[@class='toctree-l2']/a", 'Inline markup'),
        (".//title", 'Sphinx <Tests>'),
        (".//div[@class='footer']", 'Georg Brandl & Team'),
        (".//a[@href='http://python.org/']"
         "[@class='reference external']", ''),
        (".//li/p/a[@href='genindex.html']/span", 'Index'),
        (".//li/p/a[@href='py-modindex.html']/span", 'Module Index'),
        (".//li/p/a[@href='search.html']/span", 'Search Page'),
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
        (".//a[@class='footnote-reference brackets'][@href='#id8'][@id='id1']", r"1"),
        (".//a[@class='footnote-reference brackets'][@href='#id9'][@id='id2']", r"2"),
        (".//a[@class='footnote-reference brackets'][@href='#foo'][@id='id3']", r"3"),
        (".//a[@class='reference internal'][@href='#bar'][@id='id4']", r"\[bar\]"),
        (".//a[@class='footnote-reference brackets'][@href='#id10'][@id='id5']", r"4"),
        (".//a[@class='footnote-reference brackets'][@href='#id11'][@id='id6']", r"5"),
        (".//a[@class='fn-backref'][@href='#id1']", r"1"),
        (".//a[@class='fn-backref'][@href='#id2']", r"2"),
        (".//a[@class='fn-backref'][@href='#id3']", r"3"),
        (".//a[@class='fn-backref'][@href='#id4']", r"bar"),
        (".//a[@class='fn-backref'][@href='#id5']", r"4"),
        (".//a[@class='fn-backref'][@href='#id6']", r"5"),
        (".//a[@class='fn-backref'][@href='#id7']", r"6"),
    ],
    'otherext.html': [
        (".//h1", "Generated section"),
        (".//a[@href='_sources/otherext.foo.txt']", ''),
    ]
}))
@pytest.mark.sphinx('html', tags=['testtag'], confoverrides={
    'html_context.hckey_co': 'hcval_co',
    'html_experimental_html5_writer': True})
@pytest.mark.test_params(shared_result='test_build_html5_output')
def test_html5_output(app, cached_etree_parse, fname, expect):
    app.build()
    print(app.outdir / fname)
    check_xpath(cached_etree_parse(app.outdir / fname), fname, *expect)

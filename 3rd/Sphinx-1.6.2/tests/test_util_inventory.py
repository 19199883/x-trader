# -*- coding: utf-8 -*-
"""
    test_util_inventory
    ~~~~~~~~~~~~~~~~~~~

    Test inventory util functions.

    :copyright: Copyright 2007-2016 by the Sphinx team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import zlib
import posixpath

from six import BytesIO

from sphinx.ext.intersphinx import InventoryFile

inventory_v1 = '''\
# Sphinx inventory version 1
# Project: foo
# Version: 1.0
module mod foo.html
module.cls class foo.html
'''.encode('utf-8')

inventory_v2 = '''\
# Sphinx inventory version 2
# Project: foo
# Version: 2.0
# The remainder of this file is compressed with zlib.
'''.encode('utf-8') + zlib.compress('''\
module1 py:module 0 foo.html#module-module1 Long Module desc
module2 py:module 0 foo.html#module-$ -
module1.func py:function 1 sub/foo.html#$ -
CFunc c:function 2 cfunc.html#CFunc -
foo::Bar cpp:class 1 index.html#cpp_foo_bar -
foo::Bar::baz cpp:function 1 index.html#cpp_foo_bar_baz -
a term std:term -1 glossary.html#term-a-term -
ls.-l std:cmdoption 1 index.html#cmdoption-ls-l -
docname std:doc -1 docname.html -
foo js:module 1 index.html#foo -
foo.bar js:class 1 index.html#foo.bar -
foo.bar.baz js:method 1 index.html#foo.bar.baz -
foo.bar.qux js:data 1 index.html#foo.bar.qux -
a term including:colon std:term -1 glossary.html#term-a-term-including-colon -
'''.encode('utf-8'))


def test_read_inventory_v1():
    f = BytesIO(inventory_v1)
    invdata = InventoryFile.load(f, '/util', posixpath.join)
    assert invdata['py:module']['module'] == \
        ('foo', '1.0', '/util/foo.html#module-module', '-')
    assert invdata['py:class']['module.cls'] == \
        ('foo', '1.0', '/util/foo.html#module.cls', '-')


def test_read_inventory_v2():
    f = BytesIO(inventory_v2)
    invdata = InventoryFile.load(f, '/util', posixpath.join)

    assert len(invdata['py:module']) == 2
    assert invdata['py:module']['module1'] == \
        ('foo', '2.0', '/util/foo.html#module-module1', 'Long Module desc')
    assert invdata['py:module']['module2'] == \
        ('foo', '2.0', '/util/foo.html#module-module2', '-')
    assert invdata['py:function']['module1.func'][2] == \
        '/util/sub/foo.html#module1.func'
    assert invdata['c:function']['CFunc'][2] == '/util/cfunc.html#CFunc'
    assert invdata['std:term']['a term'][2] == \
        '/util/glossary.html#term-a-term'
    assert invdata['std:term']['a term including:colon'][2] == \
        '/util/glossary.html#term-a-term-including-colon'

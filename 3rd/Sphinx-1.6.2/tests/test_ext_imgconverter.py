# -*- coding: utf-8 -*-
"""
    test_ext_imgconverter
    ~~~~~~~~~~~~~~~~~~~~~

    Test sphinx.ext.imgconverter extension.

    :copyright: Copyright 2007-2017 by the Sphinx team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import pytest


@pytest.mark.sphinx('latex', testroot='ext-imgconverter')
def test_ext_imgconverter(app, status, warning):
    app.builder.build_all()

    content = (app.outdir / 'Python.tex').text()
    assert '\\sphinxincludegraphics{{svgimg}.png}' in content
    assert not (app.outdir / 'svgimg.svg').exists()
    assert (app.outdir / 'svgimg.png').exists()

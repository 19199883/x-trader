# -*- coding: utf-8 -*-
"""
    test_application
    ~~~~~~~~~~~~~~~~

    Test the Sphinx class.

    :copyright: Copyright 2007-2017 by the Sphinx team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""
from docutils import nodes

from sphinx.application import ExtensionError
from sphinx.domains import Domain

from sphinx.testing.util import strip_escseq
import pytest


def test_events(app, status, warning):
    def empty():
        pass
    with pytest.raises(ExtensionError) as excinfo:
        app.connect("invalid", empty)
    assert "Unknown event name: invalid" in str(excinfo.value)

    app.add_event("my_event")
    with pytest.raises(ExtensionError) as excinfo:
        app.add_event("my_event")
    assert "Event 'my_event' already present" in str(excinfo.value)

    def mock_callback(a_app, *args):
        assert a_app is app
        assert emit_args == args
        return "ret"
    emit_args = (1, 3, "string")
    listener_id = app.connect("my_event", mock_callback)
    assert app.emit("my_event", *emit_args) == ["ret"], "Callback not called"

    app.disconnect(listener_id)
    assert app.emit("my_event", *emit_args) == [], \
        "Callback called when disconnected"


def test_emit_with_nonascii_name_node(app, status, warning):
    node = nodes.section(names=[u'\u65e5\u672c\u8a9e'])
    app.emit('my_event', node)


def test_extensions(app, status, warning):
    app.setup_extension('shutil')
    assert strip_escseq(warning.getvalue()).startswith("WARNING: extension 'shutil'")


def test_extension_in_blacklist(app, status, warning):
    app.setup_extension('sphinxjp.themecore')
    msg = strip_escseq(warning.getvalue())
    assert msg.startswith("WARNING: the extension 'sphinxjp.themecore' was")


def test_domain_override(app, status, warning):
    class A(Domain):
        name = 'foo'

    class B(A):
        name = 'foo'

    class C(Domain):
        name = 'foo'

    # No domain know named foo.
    with pytest.raises(ExtensionError) as excinfo:
        app.override_domain(A)
    assert 'domain foo not yet registered' in str(excinfo.value)

    assert app.add_domain(A) is None
    assert app.override_domain(B) is None
    with pytest.raises(ExtensionError) as excinfo:
        app.override_domain(C)
    assert 'new domain not a subclass of registered foo domain' in str(excinfo.value)


@pytest.mark.sphinx(testroot='add_source_parser')
def test_add_source_parser(app, status, warning):
    assert set(app.config.source_suffix) == set(['.rst', '.md', '.test'])
    assert set(app.registry.get_source_parsers().keys()) == set(['*', '.md', '.test'])
    assert app.registry.get_source_parsers()['.md'].__name__ == 'DummyMarkdownParser'
    assert app.registry.get_source_parsers()['.test'].__name__ == 'TestSourceParser'

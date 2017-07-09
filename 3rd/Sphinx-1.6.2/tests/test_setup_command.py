# -*- coding: utf-8 -*-
"""
    test_setup_command
    ~~~~~~~~~~~~~~~~~~~

    Test setup_command for distutils.

    :copyright: Copyright 2007-2017 by the Sphinx team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import os
import sys
import subprocess
from collections import namedtuple
import sphinx

import pytest

from sphinx.util.osutil import cd
from textwrap import dedent


@pytest.fixture
def setup_command(request, tempdir, rootdir):
    """
    Run `setup.py build_sphinx` with args and kwargs,
    pass it to the test and clean up properly.
    """
    marker = request.node.get_marker('setup_command')
    args = marker.args if marker else []

    pkgrootdir = tempdir / 'test-setup'
    (rootdir / 'test-setup').copytree(pkgrootdir)

    with cd(pkgrootdir):
        pythonpath = os.path.dirname(os.path.dirname(sphinx.__file__))
        if os.getenv('PYTHONPATH'):
            pythonpath = os.getenv('PYTHONPATH') + os.pathsep + pythonpath
        command = [sys.executable, 'setup.py', 'build_sphinx']
        command.extend(args)

        proc = subprocess.Popen(
            command,
            env=dict(os.environ, PYTHONPATH=pythonpath),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE)
        yield namedtuple('setup', 'pkgroot,proc')(pkgrootdir, proc)


def test_build_sphinx(setup_command):
    proc = setup_command.proc
    out, err = proc.communicate()
    print(out)
    print(err)
    assert proc.returncode == 0


@pytest.mark.setup_command('-b', 'html,man')
def test_build_sphinx_multiple_builders(setup_command):
    proc = setup_command.proc
    out, err = proc.communicate()
    print(out)
    print(err)
    assert proc.returncode == 0


@pytest.mark.setup_command('-b', 'html,bar')
def test_build_sphinx_multiple_invalid_builders(setup_command):
    proc = setup_command.proc
    out, err = proc.communicate()
    print(out)
    print(err)
    assert proc.returncode == 1


@pytest.fixture
def nonascii_srcdir(request, setup_command):
    mb_name = u'\u65e5\u672c\u8a9e'
    srcdir = (setup_command.pkgroot / 'doc')
    try:
        (srcdir / mb_name).makedirs()
    except UnicodeEncodeError:
        from sphinx.testing.path import FILESYSTEMENCODING
        pytest.skip(
            'non-ASCII filename not supported on this filesystem encoding: '
            '%s' % FILESYSTEMENCODING)

    (srcdir / mb_name / (mb_name + '.txt')).write_text(dedent("""
        multi byte file name page
        ==========================
        """))

    master_doc = srcdir / 'contents.txt'
    master_doc.write_bytes((master_doc.text() + dedent("""
                            .. toctree::

                               %(mb_name)s/%(mb_name)s
                            """ % locals())).encode('utf-8'))


@pytest.mark.usefixtures('nonascii_srcdir')
def test_build_sphinx_with_nonascii_path(setup_command):
    proc = setup_command.proc
    out, err = proc.communicate()
    print(out)
    print(err)
    assert proc.returncode == 0


@pytest.mark.setup_command('-b', 'linkcheck')
def test_build_sphinx_return_nonzero_status(setup_command):
    srcdir = (setup_command.pkgroot / 'doc')
    (srcdir / 'contents.txt').write_text(
        'http://localhost.unexistentdomain/index.html')
    proc = setup_command.proc
    out, err = proc.communicate()
    print(out)
    print(err)
    assert proc.returncode != 0, 'expect non-zero status for setup.py'


def test_build_sphinx_warning_return_zero_status(setup_command):
    srcdir = (setup_command.pkgroot / 'doc')
    (srcdir / 'contents.txt').write_text(
        'See :ref:`unexisting-reference-label`')
    proc = setup_command.proc
    out, err = proc.communicate()
    print(out)
    print(err)
    assert proc.returncode == 0


@pytest.mark.setup_command('--warning-is-error')
def test_build_sphinx_warning_is_error_return_nonzero_status(setup_command):
    srcdir = (setup_command.pkgroot / 'doc')
    (srcdir / 'contents.txt').write_text(
        'See :ref:`unexisting-reference-label`')
    proc = setup_command.proc
    out, err = proc.communicate()
    print(out)
    print(err)
    assert proc.returncode != 0, 'expect non-zero status for setup.py'

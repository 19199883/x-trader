Overview
--------

[![Build Status](https://img.shields.io/travis/redjack/clogger/develop.svg)](https://travis-ci.org/redjack/clogger)

The Clogger library provides a simple, high performance logging framework based
on the ideas in the [Logbook](http://packages.python.org/Logbook/) project.

API documentation can be found [here](http://clogger.readthedocs.org/).


Build instructions
------------------

To build Windward, you need the following libraries installed on your system:

  * pkg-config
  * [libcork](https://github.com/redjack/libcork)
  * [check](http://check.sourceforge.net)

If you want to build the documentation, you also need:

  * [Sphinx](http://sphinx.pocoo.org/)

If you want to run all of the test cases, you also need:

  * [cram](http://pypi.python.org/pypi/cram/)

The Clogger library uses CMake as its build manager.  In most cases, you should
be able to build the source code using the following (from the top level of your
copy of the source tree):

    $ mkdir .build
    $ cd .build
    $ cmake .. \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_INSTALL_PREFIX=$PREFIX
    $ make
    $ make test
    $ make install

You might have to run the last command using sudo, if you need administrative
privileges to write to the `$PREFIX` directory.

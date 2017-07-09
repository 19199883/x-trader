Introduction
============

Clogger is a library for producing and handling log messages.  Its design is
heavily influenced by the Logbook_ library.

.. _Logbook: http://packages.python.org/Logbook/

Most logging frameworks these days are modeled after the Log4J_ framework, and
its equivalents in other languages.  In a Log4J-style logging framework, there
is a centralized registry of "logger objects", which are usually accessed via a
string key.  This string key identifies a particular library or subcomponent of
a library.  The user provides a centralized configuration file that provides
different logging options for different logger objects.  The string keys usually
form some kind of hierarchy, which lets the user assign logging options to an
entire tree of logger objects.

.. _Log4J: https://en.wikipedia.org/wiki/Log4j

Clogger is designed differently.  It has a complete separation between the code
that *produces* log messages and the code that *consumes* them.

On the producing side, there is no central registry of logger objects.  Users
can still assign different logging options to different sections of code,
however, because the log messages themselves have an arbitrary set of "extra
properties".  The user can handle log messages differently based on the contents
of these extra properties.

On the consuming side, there is a stack of *handlers*, each of which is allowed
to consume or augment each log message that's generated from anywhere within the
current process.  Having a stack of handlers lets you, for instance, augment
each log message produced by a network-enabled application with the IP address
of the remote client that you're communicating with.  This can be done *without*
any of your internal libraries knowing that there's an IP address being added to
the logging message.

Clogger also is designed to be highly efficient, even when a particular logging
message is not actually processed.  The intention is that is should not ever be
necessary to compile out log messages for your production and release builds,
even when you're producing log messages from within code that must process
millions of items per second.

API reference
=============

.. highlight:: c

::

    #include <clogger.h>


Log messages
------------

Log messages in Clogger contain four elements:

* a *log level*, indicating the severity of the message,

* a *channel*, which is a string indicating which library or subsystem the
  message was generated from,

* the *message* itself, which is represented by a ``printf``-style format
  string, and

* a set of *annotations* that provide information about the context of a log
  message.

This design purposefully separates the "meat" of the log message from the
context information about when and where that message occurred.  The functions
described below for generating log messages only allow the caller to provide the
level, channel, and message text.  Annotations are handled completely
orthogonally, and are added to existing log messages as they're processed.

This makes it easy to use Clogger in libraries.  Since libraries don't usually
know anything about the applications and other libraries that they're used in,
it makes sense to not require (or allow) the library to provide context
information about its logging messages.  The code that *uses* the library can
easily add annotations to any log messages produced by that library.  The end
result is that the user or system administrator can still see a full picture of
any log message that comes out of an application, while maintaining a nice
separation of concerns for the pieces that make up that application.



Log levels
----------

Clogger provides a fixed set of *log levels* that to indicate the severity of
each message:

.. type:: enum clog_level

   .. var:: CLOG_LEVEL_CRITICAL
            CLOG_LEVEL_ERROR
            CLOG_LEVEL_WARNING
            CLOG_LEVEL_NOTICE
            CLOG_LEVEL_INFO
            CLOG_LEVEL_DEBUG
            CLOG_LEVEL_TRACE

"Critical" is the most severe; "trace" is the least severe.  It's not possible
to create your own log levels.


We also provide functions for obtaining the name of a log level as a string:

.. function:: const char \*clog_level_name(enum clog_level level)
              const char \*clog_level_name_fixed_width(enum clog_level level)

   Return the name of the given log level.  The ``_fixed_width`` variant
   guarantees that all of the strings that it returns will have the same width,
   which can be useful when adding log messages to tabular outputs.


Generating log messages
-----------------------

We provide several functions for producing log messages.

.. note::

   For all of the functions in this section, we guarantee that whatever
   parameters you pass in will only be used for the duration of the function
   call.  We won't try to save those parameters into any state objects.  This
   means that it's perfectly fine to pass in stack-allocated or otherwise
   short-lived variables as your parameters.

.. function:: void clog_log_channel(enum clog_level level, const char \*channel, const char \*fmt, ...)
              void clog_log(enum clog_level level, const char \*fmt, ...)

   Generate a log message with a given severity *level*.  The text of the log
   message is given by the ``printf``-style format string *fmt* and any
   additional parameters that you provide.  For the ``_channel`` variant, the
   log message's channel is given by the *channel* parameter.  For the normal
   variant, you must define the :c:data:`CLOG_CHANNEL` macro, which will be used
   as the channel name.

   Any errors that occur while processing the log message will be silently
   caught and ignored.

.. function:: void clog_critical(const char \*fmt, ...)
              void clog_error(const char \*fmt, ...)
              void clog_warning(const char \*fmt, ...)
              void clog_notice(const char \*fmt, ...)
              void clog_info(const char \*fmt, ...)
              void clog_debug(const char \*fmt, ...)
              void clog_trace(const char \*fmt, ...)

   Helper functions for generating log levels of a particular severity.  The log
   message's channel will be taken from the :c:data:`CLOG_CHANNEL` macro, which
   you must define.  Calling any of these functions is equivalent to calling
   :c:func:`clog_log` with the respective :c:type:`clog_level` severity level.

.. var:: const char \*CLOG_CHANNEL

   A macro that's used as the channel name for any log messages created by
   :c:func:`clog_log`, :c:func:`clog_critical`, :c:func:`clog_error`,
   :c:func:`clog_warning`, :c:func:`clog_notice`, :c:func:`clog_info`, or
   :c:func:`clog_debug`.  You are responsible for defining this macro before
   using any of those functions.


Minimum severity level
~~~~~~~~~~~~~~~~~~~~~~

You can filter log messages based on arbitrary criteria via the log handler
mechanism, described below.  However, a special type of filtering is used often
enough, and needs to be as efficient as possible, so it deserves special
treatment.  This is the ability to only produce log messages above a certain
severity level.

Clogger allows you to define a *minimum severity level*; the functions described
above for producing log messages will silently ignore any message that isn't at
least as severe as the minimum severity level.  The minimum severity level can
be set with the following function:

.. function:: void clog_set_minimum_level(enum clog_level level)

   Sets the minimum severity level to *level*.  Log messages will be silently
   dropped (without passing them on to *any* log handlers) if they are not at
   least as severe as *level*.

Note that there is a single global minimum level for the entire process; this
function should only be called by an application, and not by library code.

.. admonition:: Implementation note
   :class: note

   This check is very efficient; it usually compiles down to 3-4 machine
   instructions per log message.  Most importantly, the minimum severity level
   check can be performed before the stack frame is set up for the actual call
   to the log handlers that will process the log message.  This allows us to
   always compile log messages into your applications and libraries, even if
   you need to process millions of records per second.


Handlers
--------

As mentioned above, Clogger has a clear separation between the code that
produces log messages (described in the previous section), and the code that
collects and consumes those log messages.

Clogger log messages are processed by *handlers*.  Handlers are organized into
*stacks*.  Clogger is thread-aware; each thread has its own stack of handlers.
Handlers can apply to one specific thread or to all of the threads in the
current process.  Whenever the :c:func:`clog_log` function is called from a
thread, the log message is presented to each of the handlers in that thread's
stack, in the reverse of the order that they were pushed onto the stack (i.e.,
the most recently pushed handler is called first).  A handler can abort this
process at any time, preventing the log message from being processed by any
other handlers in the stack.

This stack-based design provides a lot of flexibility.  For console
applications, you'll usually register a handler that prints log messages to
``stderr`` or sends them to the local ``syslog`` daemon.  This handler will be
the first handler that you push onto the stack.

You can also register handlers that add context information, in the form of
*annotations*, to each log message.  For instance, you could register a handler
that adds the current timestamp as an annotation, making sure to push this
handler onto the stack after the ``stderr`` handler.  (That way the annotation
handler gets executed first.)  This would cause the ``stderr`` handler to print
out the timestamp annotation as well as the body of the log message.

These annotation handlers can also implement filters.  Your handler can check
the level or content of the log message, as well as the content of any of the
annotations, to see whether to process the log message further.  If a message
doesn't pass your desired criteria, you can abort the processing of that
message.  As an example, you can register a filter handler that throws away any
log message that doesn't come from the ``"database"`` channel.  This lets you
easily limit which messages you want to view.


Registering handlers
--------------------

The functions in this section let you register and unregister log handlers.
Each handler can either apply only to the current thread, or to all of the
threads in the current process.

.. function:: void clog_handler_push_process(struct clog_handler \*handler)
              void clog_handler_push_thread(struct clog_handler \*handler)

   Push a log handler onto the stack.  With the ``_thread`` variant, the handler
   will only to the current thread.  With the ``_process`` variant, the handler
   will apply to all threads in the current process.

   It's your responsiblity to make sure that *handler* isn't already on the
   stack; if it is, the behavior is undefined.

   .. note::

      You are currently limited in that you cannot push any process handlers
      onto the stack once you've pushed any thread-specific handlers (in any
      thread).  This lets us assume that all thread-specific handlers should be
      called before any process-wide handlers.  If you call
      :c:func:`clog_handler_push_process` after calling
      :c:func:`clog_handler_push_thread`, you might get an ``assert`` failure
      (causing your process to abort).  This restriction might be removed in the
      future.

.. function:: int clog_handler_pop_process(struct clog_handler \*handler)
              int clog_handler_pop_thread(struct clog_handler \*handler)

   Pop a log handler from the top of the stack.  If *handler* isn't the handler
   at the top of the stack, then we raise a :ref:`libcork error
   <libcork:errors>` and return ``-1``.


Once you're done with a handler, you should free it:

.. function:: void clog_handler_free(struct clog_handler \*handler)

   Free *handler* and any resources that it owns.


.. _format-strings:

Format strings
--------------

Many of the built-in handlers defined below let you pass in a *format string*,
which is used to render each log message into a string for display purposes.
Each format string contains a set of *conversions* intermingled with raw text.
The raw text is copied into the result string as-is.  Each conversion is
replaced with part of the current log message.  The following conversions are
available:

``%l``
    Replaced with the log message's severity level.

``%L``
    Replaced with a fixed-width version the log message's severity level.
    (Regardless of the severity level of the log message, this conversion will
    always take up the same amount of space in the result string.)

``%c``
    Replaced with the log message's channel.

``%m``
    Replaced with the log message's text.

``%%``
    Replaced with a single ``%`` character.

``#{name}``
    Replaced with the value of the annotation named ``name``.  If there is no
    annotation with this name in the log message, this conversion is ignored.

``#!{name}{spec}``
    Replaced with the value of the annotation named ``name``.  If there is no
    annotation with this name in the log message, this conversion is ignored.
    ``spec`` is an annotation specification, described below.

``#*{spec}``
    Replaced with the values of all of the annotations in the log message.  If
    there is no annotations in the log message, this conversion is ignored.
    ``spec`` is an annotation specification, described below.

``##``
    Replaced with a single ``#`` character.


.. rubric:: Annotation specifications

Some of the conversions listed above allow you customize what is displayed for
a log message's annotation by given an *annotation specification*.  Annotation
specs are much like format strings, but with a different set of allowed
conversions:

``%k``
    Replaced with the annotation's key.

``%v``
    Replaced with the annotation's value.

``%%``
    Replaced with a single ``%`` character.


Built-in handlers
-----------------

There are a number of predefined handlers that you can register to process log
messages.


.. function:: struct clog_handler \*clog_null_handler(void)

   Return a handler that immediately drops all messages that it receives.


.. function:: struct clog_handler \*clog_stderr_handler_new(const char \*format_string)

   Return a handler that logs all messages to the ``stderr`` stream.  The format
   of each log message is controlled by the given :ref:`format string
   <format-strings>`.


.. function:: struct clog_handler \*clog_stream_handler_new_fp(FILE \*fp, bool should_close, const char \*format_string)

   Return a handler that logs all messages to the given C stream.  The format of
   each log message is controlled by the given :ref:`format string
   <format-strings>`.  If *should_close* is ``true``, then we take responsiblity
   for closing *fp* when the handler is freed.


.. function:: struct clog_handler \*clog_stream_handler_new_consumer(struct cork_stream_consumer \*consumer, const char \*format_string)

   Return a handler that logs all messages to the given :ref:`libcork stream
   consumer <libcork:stream>`.  The format of each log message is controlled by
   the given :ref:`format string <format-strings>`.  We take responsibility for
   freeing *consumer* when the handler is freed.


Filtering handler
~~~~~~~~~~~~~~~~~

You can use the :c:type:`clog_keep_filter` handler to restrict which log
channels are handled.

.. type:: struct clog_keep_filter

.. function:: struct clog_keep_filter \*clog_keep_filter_new(void)

   Create a new filtering log handler.  The handler will initially not allow any
   log messages to be processed.  You must explicitly add the channels that you
   want to process.

.. function:: void clog_keep_filter_free(struct clog_keep_filter \*filter)

   Free a filtering log handler.  Note that you can also free the handler by
   calling :c:func:`clog_handler_free` on the handler instance you get from
   :c:func:`clog_keep_filter_handler`.  The two are equivalent, and it is your
   responsibility to only free the handler once.

.. function:: void clog_keep_filter_add(struct clog_keep_filter \*filter, const char \*channel)
              void clog_keep_filter_add_many(struct clog_keep_filter \*filter, const char \*str)

   Add channel names to the filter.  The ``_add`` variant adds a single channel
   name.  The ``_add_many`` variant takes in a comma-separated list of channel
   names, and adds all of those channels to the filter.  These functions are
   idempotent: adding a channel to the filter multiple times has the same effect
   as adding it once.

.. function:: struct clog_handler \*clog_keep_filter_handler(struct clog_keep_filter \*filter)

   Return a :c:type:`clog_handler` instance for the filter.  (You must call this
   function before you can register the filter handler via
   :c:func:`clog_handler_push_process` or :c:func:`clog_handler_push_thread`.)


Writing a new handler
---------------------

.. rubric:: Handler interface

Each log handler must implement the :c:type:`clog_handler` interface.
(Typically you'll do this by embedding a :c:type:`clog_handler` instance into a
larger type.)

.. type:: struct clog_handler

   .. member:: int (\*annotation)(struct clog_handler \*handler, struct clog_message \*msg, const char \*key, const char \*value)

      Process one of the annotations for the given log message.

   .. member:: int (\*message)(struct clog_handler \*handler, struct clog_message \*msg)

      Process the body of the given log message.

   .. member:: void (\*free)(struct clog_handler \*handler)

      Free *handler* and any resources that it owns.

   .. member:: struct clog_handler \*next

      **[PRIVATE]**  You should never access this field directly, nor should you
      initialize it when constructing a new handler instance.  This field will
      be maintained and used by the stack management code.

Each handler class must implement the three methods described above.  The
:c:member:`~clog_handler.annotation` and :c:member:`~clog_handler.message`
methods should return one of the following values:

.. var:: CLOG_CONTINUE

   The message was successfully processed, and should be passed on to the next
   handler in the stack.

.. var:: CLOG_FAILURE

   There was an error processing the log message.  You should also raise a
   :ref:`libcork error condition <libcork:errors>` if you return this value.
   This could be used, for instance, in a ``syslog`` handler to signify that
   there was an error connecting with the ``syslog`` daemon.

.. var:: CLOG_SKIP

   Indicate that the current log message should not be processed further.  This
   is different from an error condition; errors are unexpected problems with the
   logging infrastructure, while a skipped message is part of the user-defined
   logic for handling log messages.

.. note::

   :c:data:`CLOG_FAILURE` will always be equal to ``-1``, and
   :c:data:`CLOG_CONTINUE` will always be equal to ``0``, since those results
   match up with the standard return values for functions that can :ref:`return
   a libcork error <libcork:errors>`.


.. rubric:: Log messages

Each log message is represented by an instance of the :c:type:`clog_message`
type.

.. type:: struct clog_message

   .. member:: enum clog_level  level

      The severity of this log message.

   .. member:: const char \*format
               va_list  args

      The text of the log message.  This is represented by a ``printf``-like
      format string (*format*) and whatever additional arguments are needed by
      the conversions in the format string.  These additional arguments are
      represented by the C99 ``va_list`` type.

There are two important things to note about the :c:type:`clog_message` type.
First, because we represent a log message's text using a ``printf`` format
string, we also need to store the arguments that are used to fill in that
string.  For performance reasons, we **don't** immediately produce the fully
formatted log message text; instead, we wait until a log handler is going to
actually do something with the fully rendered message text.  This ensures that
if a handler at the top of the stack throws away a log message while only
looking at the severity level, or one of the annotations, we don't waste the
cycles rendering the format string.

Second, we don't store the log message's annotations in the
:c:type:`clog_message` instance.  Instead, as we will see below, annotations are
only "live" for the duration of the :c:member:`~clog_handler.annotation` methods
that process the annotation.  If a handler needs an annotations to be available
after its :c:member:`~clog_handler.annotation` method returns, the handler is
responsible for making its own copy of the annotation.  This is also for
performance reasons; since most handlers will not need to save copies of the
annotations, it greatly reduces (and in most cases, eliminates) the amount of
memory that must be allocated while processing a log message.


.. rubric:: Handling a log message

When the :c:func:`clog_log` function is called, it allocates a
:c:type:`clog_message` instance (on the stack, for performance reasons), and
immediately passes control to the :c:func:`clog_process_message` function.  This
function is responsible for iterating through the stack of handlers, passing the
message to each one in turn.  It is also responsible for aborting the processing
of the message if any of the handlers requests this (via a :c:data:`CLOG_SKIP`
return code).

.. function:: int clog_process_message(struct clog_message \*msg)

   Process a log message with the current stack of handlers.  Note that you
   won't usually need to call this function directly; it's called on your behalf
   by :c:func:`clog_log`.

Each log message starts off with no annotations; it only has a severity level, a
channel string, and the text of the log message itself.  Assuming that no
annotations are added, :c:func:`clog_process_message` iterates through each of
the handlers in the current stack, calling the :c:member:`~clog_handler.message`
method of each one.  If any of the :c:member:`~clog_handler.message` methods
returns :c:data:`CLOG_FAILURE` or :c:data:`CLOG_SKIP`, we stop iterating through
the stack of handlers and immediately return.

Annotations can be added to a log message from within one of the handlers, by
calling the :c:func:`clog_annotate_message` function.  When a new annotation is
created, we immediately allow that annotation to be processed by the handlers
that are downstream of the handler that created it, by calling their
:c:member:`~clog_handler.annotation` methods.  Like with the
:c:member:`~clog_handler.message` methods, if any of the
:c:member:`~clog_handler.annotation` methods returns :c:data:`CLOG_FAILURE` or
:c:data:`CLOG_SKIP`, we immdiately abort the processing of the log message.
(The entire log message, not just this new annotation.)

.. function:: int clog_annotate_message(struct clog_handler \*handler, struct clog_message \*msg, const char \*key, const char \*value)

   Add a new annotation to *msg*.  *handler* should be the current handler ---
   i.e., the one creating the annotation.  This function won't return until the
   annotation has been fully processed by each handler in the stack.

   .. note::

      Annotations are only visible to handlers further down the stack than the
      handler that created the annotation.  Handlers that execute before
      *handler* won't have a chance to process the annotation.

There are two interesting patterns that emerge from this choreography of method
calls.  The first is that each handler will be able to process all of the
annotations that are visible to it before it processes the body of the message.
(In other words, all of the :c:member:`~clog_handler.annotation` calls for a
particular log message will happen before the :c:member:`~clog_handler.message`
call for that log message.)

The second is that creating a new annotation "interrupts" the current stage of
processing a log message.  If we're in the middle of processing the body of the
log message, the body processing is paused until the new annotation has been
fully processed.  Similarly, if we're in the middle of processing a previous
annotation, that annotation's processing is paused until the new annotation has
been fully processed.


.. rubric:: Format strings

If you would like to use a :ref:`format string <format-strings>` to render the
contents of a log message, you can create an instance of the
:c:type:`clog_formatter` type.

.. type:: struct clog_formatter

   Renders a log message into a string according to a :ref:`format string
   <format-strings>`.


.. function:: struct clog_formatter \*clog_formatter_new(const char \*format_string)

   Create a new formatter that uses *format_string* to render each log message.

.. function:: void clog_formatter_free(struct clog_formatter \*formatter)

   Free *formatter* and any resources it used.


.. function:: int clog_formatter_start(struct clog_formatter \*formatter)
              int clog_formatter_annotation(struct clog_formatter \*formatter, const char \*key, const char \*value)
              int clog_formatter_finish(struct clog_formatter \*formatter, struct clog_message \*msg, struct cork_buffer \*dest)

   These three functions are used to render a log message using a formatter.
   You first call ``_start`` first, and you must call it exactly once for each
   log message.  This function initializes some internal state in *formatter*.
   Then, you call ``_annotation`` for each annotation in the log message.
   Finally, you call ``_finish`` each with :c:type:`clog_message` instance for
   the current log message.  This final function call renders the log message
   into *dest* according to *formatter*'s format string.


As mentioned above, each log handler is guaranteed to see all of the annotations
for a log message (via its :c:member:`~clog_handler.annotation` method) before
it sees the message body (via :c:member:`~clog_handler.message`).  This means
that you can simply call :c:func:`clog_formatter_annotation` in your
:c:member:`~clog_handler.annotation` method, and call
:c:func:`clog_formatter_message` in your :c:member:`~clog_handler.message`
method.  In both function, you'll also need to check whether you've called
:c:func:`clog_formatter_start` for the current message.


Default logging setup
---------------------

Clogger provides a helper function for creating and registering a default log
handler.  This can make it much easier to get started with a new program,
minimizing the amount of work needed to see your log messages.  There's no
requirement to use the functions in this section; you might very well end up
implementation more complex, configurable control over the logging framework.


.. function:: int clog_setup_logging(void)

   Create and register a new handler that will print logging messages to stderr.
   The handler will automatically be unregistered and freed when the process
   exits.  You can control the format of the log messages using the
   :c:func:`clog_set_default_format` function.

   The user can also use environment variables (described below) to configure
   the behavior of the default handler.


.. function:: void clog_set_default_format(const char \*fmt)

   Change the format string used by the default log handler to render log
   messages.  If you never call this function, we'll use a predefined default
   format:

   .. macro:: CLOG_DEFAULT_FORMAT

      A default log format that prints out the level, channel, and message of
      each log message: ``[%L] %c: %m``


Configuring the default log handler
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The user can use environment variables to configure the behavior of the default
log handler.


.. envvar:: CLOG

   Set the minimum log level for the process.  Log messages that are less severe
   than this minimum level will not be output.  Possible (case-insensitive
   values) are: ``CRITICAL``, ``ERROR``, ``WARNING``, ``NOTICE``, ``INFO``,
   ``DEBUG``, and ``TRACE``.  If this variable is set to an invalid value, the
   :c:func:`clog_setup_logging` function will return an error.


.. envvar:: CLOG_FORMAT

   Set the :ref:`format string <format-string>` to use to render each log
   message.  If this variable is set to an invalid format string, the
   :c:func:`clog_setup_logging` function will return an error.


.. envvar:: CLOG_CHANNELS

   A comma-separated list of log channels that should be displayed.  Any log
   message with a channel not in this list will be silently dropped.  If this
   variable is not set, all log messages will be displayed.

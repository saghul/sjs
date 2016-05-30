
.. _modlog:

log
===

The `log` module provides a simple logging mechanism, backed by
`Duktape's logging framework <http://duktape.org/guide.html#logging>`_. and slightly inspired by
`Python's logging module <https://docs.python.org/2/library/logging.html>`_.

At the moment all output is sent to the system `stdout`.


Constants
---------

.. js:data:: defaultLogger

    Attribute containing the default system logger.


Functions
---------

.. js:function:: getLogger(name)

    Get a ``Logger`` object for the given `name`. When this function is called from different modules it will always
    return same object for the given `name`.

.. js:function:: trace
.. js:function:: debug
.. js:function:: info
.. js:function:: warn
.. js:function:: error
.. js:function:: fatal

    Logging functions bound to the default logger. See :ref:`loggerobj`. Example:

    ::

        sjs> log.info('hello sjs!');
        2016-04-26T10:26:26.050Z INF sjs: hello sjs!


.. _loggerobj:

The Logger object
-----------------

.. js:function:: Logger.setLevel(level)

    Set the logger's level. `level` is an integer ranging from 0 to 5:

    * 0: trace
    * 1: debug
    * 2: info
    * 3: warn
    * 4: error
    * 5: fatal

    It defaults to 2, info.

.. js:function:: Logger.trace
.. js:function:: Logger.debug
.. js:function:: Logger.info
.. js:function:: Logger.warn
.. js:function:: Logger.error
.. js:function:: Logger.fatal

    Level based log functions. They take an arbitrary number of arguments which are then formatted.

    Formatting is performed by converting each of the given arguments to a string. This can be overriden if the
    object implements a ``toLogString`` method, which will be called instead. This facilitates lazy evaluation
    for deeply nested objects which might only need to be logged for a given level.

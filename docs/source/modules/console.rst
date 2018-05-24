
.. _modconsole:

console
=======

This module provides well known logging facilities for JavaScript programs. This module (partially) implements
the `CommonJS Console specification <http://wiki.commonjs.org/wiki/Console>`_.


Console object
--------------

The `Console` object encapsulates all functionality in the `console` module in the form of a class. The module exports
a default instance of this class though more can be created (which is rarely needed).

.. js:class:: console.Console(stdout, [stderr])

    Create an instance of a console object.

    :param stdout: Object representing standard out. It must implement the same API as :js:class:`io.File`.
    :param stderr: Analogous to `stdout`, but representing the standard error. If omitted `stdout` is used.

    .. js:function:: console.Console.prototype.assert(expression, [message], [...])

        Simple assertion test using the :ref:`modassert` module. The given `expression` is evaluated and if it's
        falsey the given `message` is given with an ``AssertionError``.

        ::

            console.assert(false, 'Whoops %s', 'Houston, we\'ve got a problem!');
            // AssertionError: Whoops Houston, we've got a problem!

    .. js:function:: console.Console.prototype.log

        Outpits the given data to ``stdout`` with a newline. Data is formatted using :js:func:`utils.object.format`.

        ::

            console.log('Hello %s! Here is a number: %d and an object: %j', 'sjs', 42, {foo: 'foo', bar: 'bar'})
            // Hello sjs! Here is a number: 42 and an object: {"foo":"foo","bar":"bar"}

    .. js:function:: console.Console.prototype.info

        Alias for :js:func:`console.Console.prototype.log`.

    .. js:function:: console.Console.prototype.error

        Similar to :js:func:`console.Console.prototype.log` but it outputs to ``stderr``.

    .. js:function:: console.Console.prototype.warn

        Alias for :js:func:`console.Console.prototype.error`.

    .. js:function:: console.Console.prototype.dir(obj, [options])

        Inspect the given object using :js:func:`` and print output to ``stdout``.

        ::

			console.dir(console);
			// { log: [Function],
			//   info: [Function],
			//   warn: [Function],
			//   error: [Function],
			//   dir: [Function],
			//   time: [Function],
			//   timeEnd: [Function],
			//   trace: [Function],
			//   assert: [Function],
			//   Console: [Function: Console] }

        .. note::
            The `customInspect` option is ignored if given.

    .. js:function:: console.Console.prototype.time(label)
    .. js:function:: console.Console.prototype.timeEnd(label)

        Create a labeled timestamp. When ``timeEnd`` is called the time difference is computed and printed to
        ``staout``.

        ::

            console.time('foo');
            // (wait some time...)
            console.timeEnd('foo');
            // foo: 6535.996ms

    .. js:function:: console.Console.prototype.trace([message], [...])

        Prints a stack trace at the current position to ``stderr``. If an optional message and formatting options are
        given the message is formatted using :js:func:`utils.object.format`.

        ::

            console.trace();
            // Trace
            //     at global (<repl>:1) preventsyield


Functions
---------

.. js:function:: console.assert
.. js:function:: console.log
.. js:function:: console.info
.. js:function:: console.warn
.. js:function:: console.error
.. js:function:: console.dir
.. js:function:: console.time
.. js:function:: console.timeEnd
.. js:function:: console.trace

Functions bound to the default :js:class:`console.Console` instance.

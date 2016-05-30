
.. _modtime:

time
====

This module provides time related functionality.


Functions
---------

.. js:function:: time.sleep(secs)

    Suspends the execution of the program for the given amount of `seconds` or until a signal is received.

    .. note::
        This function may return earlier than the given amount of delay.

.. js:function:: time.time()

    Returns the time in seconds since the epoch as a floating point number.

    .. note::
        While this function normally returns non-decreasing values, it can return a lower value than a
        previous call if the system clock has been set back between the two calls.
        :js:func:`time.hrtime` can be used instead, which doesn't suffer from this.

.. js:function:: time.hrtime([start])

    Returns the current high-resolution real time in a ``[seconds, nanoseconds]`` tuple Array. It is relative to
    an arbitrary time in the past. It is not related to the time of day and therefore not subject to clock drift.
    The primary use is for measuring performance between intervals.

    You may pass in the result of a previous call to this function to get a diff reading, useful for benchmarks
    and measuring intervals:

    ::

        var start = time.hrtime();
        time.sleep(1);
        time.hrtime(start);
        [1,3976731]

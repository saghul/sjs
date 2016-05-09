
.. _modio:

io
==

This module provies access to i/o primitives.


.. js:data:: io.stdin

    Object of type :js:class:`fs.File` representing the standard input.

.. js:data:: io.stdout

    Object of type :js:class:`fs.File` representing the standard output.

.. js:data:: io.stderr

    Object of type :js:class:`fs.File` representing the standard error.


io/select
---------

This submodule provides access to :man:`select(2)`.

.. js:function:: select.select(rfds, wfds, xfds, timeout)

    Wait until any of the given file descriptors are ready for reading, writing or have a pending exceptional
    condition.

    :param rfds: Array of file descriptors to monitor for reading.
    :param wfds: Array of file descriptors to monitor for writing.
    :param xfds: Array of file descriptors to monitor for pending exceptional conditions.
    :param timeout: Amount of time to wait. ``null`` means unlimited. This function might return early if interrupted
        by a signal.
    :returns: An object containing 3 properties: `rfds`, `wfds` and `xfds`, containing the file descriptors which are
        ready for each condition respectively.

    For more information see :man:`select(2)`.

io/poll
-------

This submodule provides access to :man:`poll(2)`.

.. js:data:: poll.POLLIN
.. js:data:: poll.POLLOUT
.. js:data:: poll.POLLPRI
.. js:data:: poll.POLLRDHUP
.. js:data:: poll.POLLERR
.. js:data:: poll.POLLHUP
.. js:data:: poll.POLLINVAL

    Constants to be used in the `events` or `revents` fields of a ``pollfd`` object. Check :man:`poll(2)` for
    more information. Note that not all these constants might be available on your platform.

.. js:function:: poll.poll(pfds, timeout)

    Examines the given file descriptors to see if some of them are ready for i/o or if certain events have
    occurred on them.

    :param pfds: An array of ``pollfd`` objects to be examined. A ``pollfd`` object is any object which has a `fd` and
        a `events` properties. The `events` property must contain the or-ed events that the user is interested in
        examining.
    :param timeout: Amount of time to wait. ``null`` means unlimited. This function might return early if interrupted
        by a signal.
    :returns: An array of ``pollfd`` objects, containing `fd`, `events` and `revents` properties. `fd` and `events`
        match the given ones, and `revents` indicates the received events.

    For more information see :man:`poll(2)`.

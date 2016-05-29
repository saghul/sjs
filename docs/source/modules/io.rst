
.. _modio:

io
==

This module provies access to high i/o primitives and streams.


File object
-----------

.. js:class:: io.File

    Class representing a file stream. This class created through :js:func:`io.open` or :js:func:`io.fdopen`.
    The :ref:`modio` module also makes use of this class for wrapping
    `stdio <https://en.wikipedia.org/wiki/C_file_input/output>`_ streams.

.. js:attribute:: io.File.path

    Returns the opened file's path.

.. js:attribute:: io.File.fd

    Returns the file descriptor associated with the file.

.. js:attribute:: io.File.mode

    Returns the mode in which the file was opened.

.. js:attribute:: io.File.closed

    Boolean flag indicating if the file was closed.

.. js:function:: io.FIle.prototype.read(nread)

    Read data from the file.

    :param nread: Amount of data to receive. If not specified it defaults to 4096. Alternatively, a `Buffer`
        can be passed, and data will be read into it.
    :returns: The data that was read as a string or the amount of data read as a number, if a `Buffer` was passed.

    .. seealso::
        :man:`fread(3)`

.. js:function:: io.FIle.prototype.readline(nread)

    Similar to :js:func:`io.FIle.prototype.read`, but stops at the newline (``\n``) character.
    This is the recommended function to read from `stdin`, but not from binary files.

    .. seealso::
        :man:`fgets(3)`

.. js:function:: io.FIle.prototype.write(data)

    Write data on the file.

    :param data: The data that will be written (can be a string or a `Buffer`).
    :returns: The number of bytes from `data` which were actually written.

    .. seealso::
        :man:`fwrite(3)`

.. js:function:: io.FIle.prototype.flush

    Flush the buffered write data to the file.

    .. seealso::
        :man:`fflush(3)`

.. js:function:: io.FIle.prototype.close

    Close the file.


Functions / attributes
----------------------

.. js:function:: io.open(path, mode)

    Opens the file at the given `path` in the given mode. Check :man:`fopen(3)` for the `mode` details.
    It returns a :js:class:`io.File` object.

.. js:function:: io.fdopen(fd, mode, [path])

    Opens the fiven file descriptor in `fd` as a :js:class:`io.File` object. The given `mode` must be compatible with
    how the file descriptor was opened. `path` is purely informational.

    .. seealso::
        :man:`fdopen(3)`

.. js:data:: io.stdin

    Object of type :js:class:`io.File` representing the standard input.

.. js:data:: io.stdout

    Object of type :js:class:`io.File` representing the standard output.

.. js:data:: io.stderr

    Object of type :js:class:`io.File` representing the standard error.


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


.. _modfs:

fs
==

This module provides access to file system related utilities.


File object
-----------

.. js:class:: fs.File

    Class representing a file stream. This class created through :js:func:`fs.open` or :js:func:`fs.fdopen`.
    The :ref:`modio` module also makes use of this class for wrapping
    `stdio <https://en.wikipedia.org/wiki/C_file_input/output>`_ streams.

.. js:attribute:: fs.File.path

    Returns the opened file's path.

.. js:attribute:: fs.File.fd

    Returns the file descriptor associated with the file.

.. js:attribute:: fs.File.mode

    Returns the mode in which the file was opened.

.. js:attribute:: fs.File.closed

    Boolean flag indicating if the file was closed.

.. js:function:: fs.FIle.prototype.read(nread)

    Read data from the file.

    :param nread: Amount of data to receive. If not specified it defaults to 4096. Alternatively, a `Buffer`
        can be passed, and data will be read into it.
    :returns: The data that was read as a string or the amount of data read as a number, if a `Buffer` was passed.

    .. seealso::
        :man:`fread(3)`

.. js:function:: fs.FIle.prototype.write(data)

    Write data on the file.

    :param data: The data that will be written (can be a string or a `Buffer`).
    :returns: The number of bytes from `data` which were actually written.

    .. seealso::
        :man:`fwrite(3)`

.. js:function:: fs.FIle.prototype.flush

    Flush the buffered write data to the file.

    .. seealso::
        :man:`fflush(3)`

.. js:function:: fs.FIle.prototype.close

    Close the file.


Functions
---------

.. js:function:: fs.open(path, mode)

    Opens the file at the given `path` in the given mode. Check :man:`fopen(3)` for the `mode` details.
    It returns a :js:class:`fs.File` object.

.. js:function:: fs.fdopen(fd, mode, [path])

    Opens the fiven file descriptor in `fd` as a :js:class:`fs.File` object. The given `mode` must be compatible with
    how the file descriptor was opened. `path` is purely informational.

    .. seealso::
        :man:`fdopen(3)`

.. js:function:: fs.unlink(path)

    Unlinks (usually this means completely removing) the given `path`.

    .. seealso::
        :man:`unlink(3)`

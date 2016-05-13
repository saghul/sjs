
.. _modos:

os
==

This module exposes low level operating system facilities / syscalls.


.. js:function:: os.abort

    Aborts the execution of the process and forces a core dump.

    .. seealso::
        :man:`abort(3)`

.. js:function:: os.close(fd)

    Close the given file descriptor.

    .. seealso::
        :man:`close(2)`

.. js:function:: os.isatty(fd)

    Returns ``true`` if the given `fd` refers to a valid terminal type device, ``false`` otherwise.

    .. seealso::
        :man:`isatty(3)`

.. js:function:: os.open(path, flags, mode)

    Opens a file.

    :param path: The file path to be opened.
    :param flags: How the file will be opened. It can be a string or an OR-ed mask of constants (listed below). Here
        are the supported possibilities:

        * 'r' = ``O_RDONLY``: open the file just for reading
        * 'r+' = ``O_RDWR``: open the file for reading and writing
        * 'w' = ``O_TRUNC | O_CREAT | O_WRONLY``: open the file for writing only, truncating it if it exists and
          creating it otherwise
        * 'wx' = ``O_TRUNC | O_CREAT | O_WRONLY | O_EXCL``: like 'w', but fails if the path exists
        * 'w+' = ``O_TRUNC | O_CREAT | O_RDWR``: open the file for reading and writing, truncating it if it exists and
          creating it otherwise
        * 'wx+' = ``O_TRUNC | O_CREAT | O_RDWR | O_EXCL``: like 'w+' but fails if the path exists
        * 'a' = ``O_APPEND | O_CREAT | O_WRONLY``: open the file for apending, creating it if it doesn't exist
        * 'ax' = ``O_APPEND | O_CREAT | O_WRONLY | O_EXCL``: like 'a' but fails if the path exists
        * 'a+' = ``O_APPEND | O_CREAT | O_RDWR``: open the file for reading and apending, creating it if it
          doesn't exist
        * 'ax+' = ``O_APPEND | O_CREAT | O_RDWR | O_EXCL``: like 'a+' but fails if the path exists
    :param mode: Sets the file mode (permissions and sticky bits).
    :returns: The opened file descriptor.

    .. seealso::
        :man:`open(2)`

.. js:function:: os.pipe

    Creates a `pipe` (an object that allows unidirectional data flow) and allocates a pair of file descriptors.
    The first descriptor connects to the read end of the pipe; the second connects to the write end. File descriptors
    are returned in an array: ``[read_fd, write_fd]``.

    .. seealso::
        :man:`pipe(2)`

.. js:function:: os.read([nread])

    Read data from the file descriptor.

    :param nread: Amount of data to receive. If not specified it defaults to 4096. Alternatively, a `Buffer`
        can be passed, and data will be read into it.
    :returns: The data that was read as a string or the amount of data read as a number, if a `Buffer` was passed.

    .. seealso::
        :man:`read(2)`

.. js:function:: os.ttyname(fd)

    Returns the related device name of the given `fd` for which :js:func:`os.isatty` is ``true``.

    .. seealso::
        :man:`ttyname(3)`

.. js:function:: os.write(data)

    Write data on the file descriptor.

    :param data: The data that will be written (can be a string or a `Buffer`).
    :returns: The number of bytes from `data` which were actually written.

    .. seealso::
        :man:`write(2)`

.. js:attribute:: os.O_*

    Constants used as flags in :js:func:`os.open`.

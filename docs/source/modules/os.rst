
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

.. js:function:: os.scandir(path)

    Lists all files in the given `path`.

    .. seealso::
        :man:`scandir(3)`

.. js:function:: os.stat(path)

    Obtain information about the file pointed to by `path`.

    Returns an object with the following properties:

    * dev
    * mode
    * nlink
    * uid
    * gid
    * rdev
    * ino
    * size
    * blksize
    * blocks
    * flags
    * gen
    * atime
    * mtime
    * ctime
    * birthtime

    The ``atime``, ``mtime``, ``ctime`` and ``birthtime`` fields are of type
    `Date <https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date>`_.

    .. seealso::
        :man:`stat(2)`

.. js:function:: os.ttyname(fd)

    Returns the related device name of the given `fd` for which :js:func:`os.isatty` is ``true``.

    .. seealso::
        :man:`ttyname(3)`

.. js:function:: os.unlink(path)

    Unlinks (usually this means completely removing) the given `path`.

    .. seealso::
        :man:`unlink(3)`

.. js:function:: os.urandom(bytes)

    Get `bytes` from the system `CSPRNG <https://en.wikipedia.org/wiki/Cryptographically_secure_pseudorandom_number_generator>`_.
    This is implemented by reading from ``/dev/urandom`` except on Linux systems supporting the :man:`getrandom(2)`
    syscall.

    `bytes` can be an integer or a ``Buffer`` object. If it's an integer a ``Buffer`` will be returned of the specified
    size. If it's already a ``Buffer``, if will be filled.

.. js:function:: os.write(data)

    Write data on the file descriptor.

    :param data: The data that will be written (can be a string or a `Buffer`).
    :returns: The number of bytes from `data` which were actually written.

    .. seealso::
        :man:`write(2)`

.. js:function:: os.S_IMODE(mode)

    Returns the permissions bits out of the mode field obtained with :js:func:`os.stat`.

.. js:function:: os.S_ISDIR(mode)

    Returns ``true`` if the `mode` of the file indicates it's a directory.

.. js:function:: os.S_ISCHR(mode)

    Returns ``true`` if the `mode` of the file indicates it's a character device.

.. js:function:: os.S_ISBLK(mode)

    Returns ``true`` if the `mode` of the file indicates it's a block device.

.. js:function:: os.S_ISREG(mode)

    Returns ``true`` if the `mode` of the file indicates it's a regular file.

.. js:function:: os.S_ISFIFO(mode)

    Returns ``true`` if the `mode` of the file indicates it's a FIFO.

.. js:function:: os.S_ISLINK(mode)

    Returns ``true`` if the `mode` of the file indicates it's a symbolic link.

.. js:function:: os.S_ISSOCK(mode)

    Returns ``true`` if the `mode` of the file indicates it's a socket.


Constants
^^^^^^^^^

.. js:attribute:: os.O_*

    Constants used as flags in :js:func:`os.open`.

.. js:attribute:: os.S_IF*

    Flags used to check the file type in :js:func:`os.stat`.

.. js:attribute:: os.S_I*

    Flags for file mode used in :js:func:`os.stat`.

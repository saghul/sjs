
.. _modos:

os
==

This module exposes low level operating system facilities / syscalls.


Functions
---------

.. js:function:: os.abort

    Aborts the execution of the process and forces a core dump.

    .. seealso::
        :man:`abort(3)`

.. js:function:: os.chdir(path)

    Changes the current working directory of the calling process to the directory specified in `path`.

    .. seealso::
        :man:`chdir(2)`

.. js:function:: os.cloexec(fd, set)

    Sets or clears the ``O_CLOEXEC`` flag on the given `fd`. Since version 0.3.0 all fds are created with
    ``O_CLOEXEC`` set.

.. js:function:: os.close(fd)

    Close the given file descriptor.

    .. seealso::
        :man:`close(2)`

.. js:function:: os.dup(oldfd)

    Duplicates the given file descriptor, returning the lowest available one.

    .. seealso::
        :man:`dup(2)`

    .. note::
        Since version 0.3.0 the fds are created with ``O_CLOEXEC`` set. You can undo this using :js:func:`os.cloexec`.

.. js:function:: os.dup2(oldfd, newfd, [cloexec])

    Duplicates `oldfd` into `newfd`, setting the ``O_CLOEXEC`` flag if indicated. It defaults to ``true``;

    .. seealso::
        :man:`dup2(2)`

.. js:function:: os.execve(filename, [args], [envp])

    Executes the program pointed to by `filename`.

    :param filename: Program to be executed.
    :param args: Arguments for the program. If an ``Array`` is passed, the first element should be the
        program filename.
    :param envp: Object containing the environment for the new program.

    .. seealso::
        :man:`execve(2)`

.. js:function:: os.exit([status])

    Ends the process with the specified `status`. It defaults to 0.

    .. seealso::
        :man:`exit(3)`

    .. note::
        At the moment no clean shutdown is performed.

.. js:function:: os._exit(status)

    Terminate the calling process "immediately".

    .. seealso::
        :man:`_exit(2)`

.. js:function:: os.fork

    Creates a new process duplicating the calling process. See :js:func:`os.waitpid` for how to wait for the
    child process.

    .. seealso::
        :man:`fork(2)`

.. js:function:: os:getpid

    Returns the process id of the calling process.

    .. seealso::
        :man:`getpid(2)`

.. js:function:: os:getppid

    Returns the process id of the parent of the calling process.

    .. seealso::
        :man:`getppid(2)`

.. js:function:: os.isatty(fd)

    Returns ``true`` if the given `fd` refers to a valid terminal type device, ``false`` otherwise.

    .. seealso::
        :man:`isatty(3)`

.. js:function:: os.nonblock(fd, set)

    Sets or clears the ``O_NONBLOCK`` flag on the given `fd`.

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

    .. note::
        Since version 0.3.0 the fds are created with ``O_CLOEXEC`` set. You can undo this using :js:func:`os.cloexec`.

.. js:function:: os.pipe

    Creates a `pipe` (an object that allows unidirectional data flow) and allocates a pair of file descriptors.
    The first descriptor connects to the read end of the pipe; the second connects to the write end. File descriptors
    are returned in an array: ``[read_fd, write_fd]``.

    .. seealso::
        :man:`pipe(2)`

    .. note::
        Since version 0.3.0 the fds are created with ``O_CLOEXEC`` set. You can undo this using :js:func:`os.cloexec`.

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

.. js:function:: os.setsid

    Create a new session if the calling process is not a process group leader.

    .. seealso::
        :man:`setsid(2)`

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
    This is implemented by reading from ``/dev/urandom``. On Linux systems supporting the :man:`getrandom(2)`
    syscall that one is used, and in OSX :man:`arc4random_buf(3)`.

    `bytes` can be an integer or a ``Buffer`` object. If it's an integer a ``Buffer`` will be returned of the specified
    size. If it's already a ``Buffer``, if will be filled.

.. js:function:: os.waitpid(pid, [options])

    Wait for state changes in a child of the calling process. The return value is an object with ``pid`` and ``status``
    properties. The ``os.W*`` family of functions can be used to get more information about the status.

    .. seealso::
        :man:`waitpid(2)`

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

.. js:function:: os.WIFEXITED(status)
.. js:function:: os.WEXITSTATUS(status)
.. js:function:: os.WIFSIGNALED(status)
.. js:function:: os.WTERMSIG(status)
.. js:function:: os.WIFSTOPPED(status)
.. js:function:: os.WSTOPSIG(status)
.. js:function:: os.WIFCONTINUED(status)

    Helper functions to get status information from a child process. See the man page: :man:`waitpid(2)`.


Constants
---------

.. js:attribute:: os.O_*

    Constants used as flags in :js:func:`os.open`.

.. js:attribute:: os.S_IF*

    Flags used to check the file type in :js:func:`os.stat`.

.. js:attribute:: os.S_I*

    Flags for file mode used in :js:func:`os.stat`.

.. js:attribute:: os.W*

    Flags used in the options field on :js:func:`os.waitpid`.

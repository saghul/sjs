
.. _modprocess:

process
=======

This module provides Unix process management utilities.


Process object
--------------

The `Process` object represents a child process which was spawned and provides the necessary tools to
control it.

.. js:attribute:: process.Process.pid

    The process ID of the child process. Note that if ``shell: true`` was specified when spawning, the returned ID
    is that of the shell.

.. js:class:: process.Process(options)

    Creates a process object. Process objects are created with the :js:func:`process.spawn` factory function.

.. js:attribute:: process.Process.stdin

    :js:class:`io.File` object representing the child process' `stdin`. It's write-only and any data written to
    it will show up as input data in the child process.

.. js:attribute:: process.Process.stdout

    :js:class:`io.File` object representing the child process' `stdout`. It's read-only and any data the child process
    writes to its standard output will be available to read.

.. js:attribute:: process.Process.stdin

    :js:class:`io.File` object representing the child process' `stderr`. It's read-only and any data the child process
    writes to its standard error will be available to read.

.. js:attribute:: process.Process.wait

    Wait untill the child process is finished. Returns an object with 2 properties:

    * exit_status: the status code when the process exited. If the process didn't exit normally or due to
      a call to :man:`exit(3)` (os :man:`exit(2)`) the value will be 0.
    * term_signal: the number of the signal that caused the child process to terminate. If the process didn't
      terminate because of a signal the value will be 0.


Functions
---------

.. js:function:: process.daemonize

    Detaches the current process from the terminal and continues to run in the background as a system daemon.
    This is perfomed using the typical Unix double-fork approach. The working directory is changed to ``/`` and all
    `stdio` file descriptors are replaced with ``/dev/null``. Similar to :man:`daemon(3)`.

.. js:function:: process.spawn(cmd, [options])

    Creates a child process to run the given command. `cmd` should be an ``Array`` with the shell-escaped arguments
    or a string containing the full command, if the ``shell`` option is used.

    The `options` object customizes how the child process is executed. The following properties are supported:

    * `cwd`: working directory for the new process.
    * `env`: object containing the environment for the new process. The calling process' environment will be
      used in case ``null`` is provided.
    * `shell`: if ``true`` a shell will be used to spawn the command, thus running ``/bin/sh -c cmd``.
    * `uid`: the effective user ID for the new process.
    * `gid`: the effective group ID for the new process.

    Example:

    ::

		sjs> const proc = require('process');
		= undefined
		sjs> var p = proc.spawn(['ls', '-l'], {stdin: null, stdout: 'pipe', stderr: null});
		= undefined
		sjs> print(p.stdout.read());
		total 56
		-rw-r--r-- 1 saghul saghul  117 May  7 23:40 AUTHORS
		drwxr-xr-x 3 saghul saghul 4096 Jun  6 09:17 build
		-rw-r--r-- 1 saghul saghul 3021 May 17 01:22 CHANGELOG.md
		-rw-r--r-- 1 saghul saghul 3938 Jun  4 18:16 CMakeLists.txt
		-rw-r--r-- 1 saghul saghul  619 May  2 11:02 CONTRIBUTING.md
		drwxr-xr-x 4 saghul saghul 4096 Jun  2 23:44 docs
		drwxr-xr-x 3 saghul saghul 4096 Mar 18 12:50 include
		-rw-r--r-- 1 saghul saghul 1080 May  1 23:30 LICENSE
		-rw-r--r-- 1 saghul saghul  667 May  6 01:38 Makefile
		drwxr-xr-x 4 saghul saghul 4096 Jun  6 02:41 modules
		-rw-r--r-- 1 saghul saghul 2829 Jun  3 00:52 README.md
		drwxr-xr-x 4 saghul saghul 4096 Jun  2 23:42 src
		drwxr-xr-x 3 saghul saghul 4096 Jun  6 09:14 test
		drwxr-xr-x 2 saghul saghul 4096 Apr 22 02:16 tools

		= undefined
		sjs>

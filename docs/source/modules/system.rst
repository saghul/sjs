
.. _modsystem:

system
======

This module provides information about the system where `sjs` is running as well as the
environment.


.. js:data:: system.versions

    An object containing information about the `sjs` and embedded `Duktape` versions.

    ::

        = { duktape: 'v1.4.0-356-gf1e68db',
        duktapeCommit: 'f1e68db',
        sjs: '0.0.1dev' }

.. js:data:: system.env

    Array containing the current environment variables.

.. js:data:: system.path

    Array containing the list of locations which are going to be used when searching for modules. It can be modified
    at runtime.

    A list of colon separated paths can also be specified with the ``SJS_PATH`` variable, which will be prepended
    to the default paths list.

.. js:data:: system.arch

    System architecture (``x86``, ``x64`` or ``arm``).

.. js:data:: system.platform

    String representing the running platform (``linux`` or ``osx``).

.. js:data:: system.executable

    Absolute path of the executable that started the process.

.. js:data:: system.argv

    Set of command line arguments that were given to the process.

.. js:function:: system.exit(code)

    Ends the process with the specified `code`.

    .. note::
        At the moment no clean shutdown is performed.

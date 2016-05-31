
.. _modsystem:

system
======

This module provides information about the system where `sjs` is running as well as the
environment.


Attributes
----------

.. js:data:: system.versions

    An object containing information about the `sjs` and embedded `Duktape` versions.

    ::

        sjs> print(JSON.stringify(system.versions, null, 4))
        {
            "duktape": "v1.5.0",
            "duktapeCommit": "83d5577",
            "sjs": "0.1.0"
        }

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

.. js:data:: system.build

    Object providing various information about the build and the system where it was produced:

    ::

        sjs> print(JSON.stringify(system.build, null, 4))
        {
            "compiler": "GCC",
            "compilerVersion": "5.3.1",
            "system": "Linux-4.5.0-1-amd64",
            "cflags": "-pedantic -std=c99 -Wall -fstrict-aliasing -fno-omit-frame-pointer -Wextra -O0 -g3",
            "timestamp": "2016-05-07T17:37:46Z",
            "type": "Debug"
        }

.. js:data:: system.endianness

    Returns ``big`` if the system is Big Engian, or ``little`` if the system is Little Endian. This is determined at runtime.

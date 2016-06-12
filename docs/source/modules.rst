
.. _modules:

Modules
=======

`sjs` includes a builtin `Common JS <http://wiki.commonjs.org/wiki/Modules/1.1>`_ module loading system.
There is also a :ref:`stdlib` whith many modules. The goal is to provide a `kitchen-sink` collection of modules.

Starting with version 0.4.0, `sjs` includes a module system more similar to Node.


.. _stdlib:

Standard library
----------------

.. toctree::
    :maxdepth: 1

    modules/assert
    modules/codecs
    modules/errno
    modules/io
    modules/log
    modules/net
    modules/objectutil
    modules/os
    modules/path
    modules/process
    modules/pwd
    modules/random
    modules/system
    modules/time
    modules/uuid


Builtins
--------

These functions / objects are accessible globally.

.. js:data:: global

    A reference to the global scope. This can be used to attach functions / objects which will be accesible globally.

.. js:function:: require(id)

    Function for loading a `Common JS <http://wiki.commonjs.org/wiki/Modules/1.1>`_ module.

.. js:function:: print(data)

    Utility function to write `data` to stdout.

.. js:function:: alert(data)

    Utility function to write `data` to stdout.

.. js:data:: __filename

    A sort of global attribute containing the absolute path to the file. In the global scope it constains the filename
    which is currently being executed, ``<repl>`` if running in the REPL, ``<stdin>`` if code is being read from `stdin`,
    or ``<eval>`` if evaluating code straight from the CLI. Inside a module, it contains the absolute path to the
    module file.

.. js:data:: __dirname

    The directory of the file being evaluated, obtained by applying :man:`dirname(3)` over ``__filename``.

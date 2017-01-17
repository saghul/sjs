
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
    modules/console
    modules/errno
    modules/hash
    modules/io
    modules/net
    modules/os
    modules/path
    modules/process
    modules/pwd
    modules/random
    modules/system
    modules/time
    modules/utils
    modules/uuid


Builtins
--------

These functions / objects are accessible globally.

.. js:data:: global

    A reference to the global scope. This can be used to attach functions / objects which will be accesible globally.

.. js:function:: require(id)

    Function for loading a `Common JS <http://wiki.commonjs.org/wiki/Modules/1.1>`_ module. See :ref:`module_system`.

.. js:function:: print(data)

    Utility function to write `data` to stdout.

.. js:function:: alert(data)

    Utility function to write `data` to stdout.

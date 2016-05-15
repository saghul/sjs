
.. _modules:

Modules
=======

`sjs` includes a builtin `Common JS <http://wiki.commonjs.org/wiki/Modules/1.1>`_ module loading system.
There is also a :ref:`stdlib` whith many modules. The goal is to provide a `kitchen-sink` collection of modules.


Builtins
--------

These functions / objects are accessible globally.

.. js:data:: global

    A reference to the global scope. This can be used to attach functions / objects which will be accesible globally.

.. js:data:: system

    A reference to the :ref:`modsystem` module.

.. js:function:: require(id)

    Function for loading a `Common JS <http://wiki.commonjs.org/wiki/Modules/1.1>`_ module.

.. js:function:: print(data)

    Utility function to write `data` to stdout.

.. js:function:: alert(data)

    Utility function to write `data` to stdout.


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
    modules/system
    modules/time

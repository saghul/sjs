
.. _module_system:

The module system
=================

`sjs` uses a `CommonJS <http://wiki.commonjs.org/wiki/Modules/1.1>`_ compliant module system, also inspired by
`the Node module system <https://nodejs.org/api/modules.html>`_.


Loading modules
---------------

Modules are loaded using the :js:func:`require` function. Given a *module id*, it returns the module exported
data.

The module id can be a relative path or a non-relative path, in which case the module is loaded from the system
search directories. See :ref:`modsearchpath`.


Module context
--------------

When a module is being loaded the following *module globals* are available:

.. js:attribute:: __filename

    Path to the file being executed. In the global scope it constains the filename which is currently being executed,
    ``<repl>`` if running in the REPL, ``<stdin>`` if code is being read from `stdin`, or ``<eval>`` if evaluating
    code straight from the CLI. Inside a module, it contains the absolute path to the module file.

.. js:attribute:: __dirname

    Directory where the file being evaluated is, obtained by applying :man:`dirname(3)` over ``__filename``.

.. js:attribute:: module

    The current :js:class:`Module` object instance.

.. js:attribute:: exports

    The current module exports. It's a reference to ``module.exports``.

.. js:function:: require(id)

    Loads the requested module and returns the module's exports.

    Example, assuming some ``foo.js`` file with the following content:

    ::

        function foo() {
            return 42;
        }

        exports.foo = foo;

    It can be loaded and used as follows:

    ::

        const mod = require('./foo');

        print(mod.foo());
        // prints 42


The Module object
^^^^^^^^^^^^^^^^^

.. js:class:: Module

    Object representing a JavaScript module (for the lack of a better term).

    .. js:attribute:: Module.filename

    Fully resolved filename of the module.

    .. js:attribute:: Module.id

    Same as ``filename``.

    .. js:attribute:: Module.loaded

    Boolean attribute indicated if the module was loaded or if it's in the process of being loaded.

    .. js:attribute:: Module.exports

    Object containing the functions and attributes to be exported.


The "main" module
^^^^^^^^^^^^^^^^^

The :js:func:`require` function has a ``main`` attribute, referencing the current module only for the "main" module.
Otherwise it's ``undefined``.

The following construct can be used in order to differentiate if a module was require()-d or directly run:

::

    if (require.main === module) {
        // module was directly run
    }


.. _modsearchpath:

Module search paths
-------------------

Modules are located by their *module id*. This module id can be one of:

- a relative path: ex. ``./foo`` or ``../foo``
- a regular module id: ex. ``system``

Absolute paths are not supported.

Relative paths are resolved relative to the *calling* module, or the module which contains the call to
:js:func:`require`.

Regular module ids are resolved by looking into the :ref:`modsystem` paths in ``system.path``. The list of paths
to search for modules is dynamic and can be modified at runtime. The following are the builtin system paths:

- ``/usr/lib/sjs/modules``
- ``/usr/local/lib/sjs/modules``
- ``~/.local/sjs/modules``

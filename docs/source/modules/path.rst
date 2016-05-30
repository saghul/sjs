
.. _modpath:

path
====

This module provides path manipulation related functions.


Functions
---------

.. js:function:: path.basename(p)

    Returns the last component from the pathname in `path`, deleting any trailing ``/`` characters.

    ::

        path.basename('foo/bar/baz.js');
        = baz.js

    .. seealso::
        :man:`basename(3)`.

.. js:function:: path.dirname(p)

    The converse function of :js:func:`path.basename`. Returns the parent directory of the pathname in `path`.

    ::

        path.dirname('foo/bar/baz.js');
        = foo/bar

    .. seealso::
        :man:`dirname(3)`.

.. js:function:: path.join(...)

    Joins the given list of partial paths into a single one using ``/`` as the separator.

    ::

        path.join('foo', 'bar', 'baz');
        = foo/bar/baz

.. js:function:: path.normalize(p)

    `Normalize` the given path by performing tilde expansion and then applying :man:`realpath(3)`. In case of error it
    returns the given path unchanged.

    ::

        path.normalize('~/src/sjs');
        = /Users/saghul/src/sjs

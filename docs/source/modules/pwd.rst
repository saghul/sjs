
.. _modpwd:

pwd
===

This module provides functions to query the Unix password database.


passwd object
-------------

These objects are returned by the functions in this module and represent an entry in the password database.
They are a thin wrapper around a ``struct passwd`` structure.

Properties:

* name: user name
* passwd: user password (typically 'x', since it's encrypted)
* uid: user ID
* gid: group ID
* gecos: user information / description
* dir: user home directory
* shell: user shell


Functions
---------

.. js:function:: pwd.getpwnam(name)

    Returns the passwd object mathing the given `name` in the Unix password database.

    .. seealso::
        :man:`getpwnam(3)`.

.. js:function:: pwd.getpwuid(uid)

    Returns the passwd object mathing the given `uid` in the Unix password database.

    .. seealso::
        :man:`getpwuid(3)`.

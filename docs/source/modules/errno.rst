
.. _moderrno:

errno
=====

The `errno` module exposes all system error codes matching reason strings.

.. note::
    The exposed error codes may vary depending on the platform.


.. js:data:: map

    A ``Map`` mapping error codes to their string versions. Example:

    ::

        sjs> errno.map.get(1)
        = EPERM

.. js:data:: E*

    All errno constants available in the system as exposed as module level constants. Example:

    ::

        sjs> errno.EPERM
        = 1

.. js:function:: strerror(code)

    Get the string that describes the given error `code`.

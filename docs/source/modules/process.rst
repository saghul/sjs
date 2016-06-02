
.. _modprocess:

process
=======

This module provides Unix process management utilities.


Functions
---------

.. js:function:: process.daemonize

    Detaches the current process from the terminal and continues to run in the background as a system daemon.
    This is perfomed using the typical Unix double-fork approach. The working directory is changed to ``/`` and all
    `stdio` file descriptors are replaced with ``/dev/null``. Similar to :man:`daemon(3)`.

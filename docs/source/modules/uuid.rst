
.. _moduuid:

uuid
====

This module provides UUID generating and parsing functions.
I was adaped from `this module <https://github.com/defunctzombie/node-uuid>`_.


Functions
---------

.. js:function:: uuid.v1([options [, buffer [, offset]]])

    Generate and return a RFC4122 v1 (timestamp-based) UUID.

    :param options: Optional object with uuid state to apply. Properties may include:

       	- node - (Array) Node id as Array of 6 bytes (per 4.1.6). Default: Randomly generated ID. See note 1.
        - clockseq - (Number between 0 - 0x3fff) RFC clock sequence. Default: An internally maintained clockseq is used.
        - msecs - (Number | Date) Time in milliseconds since unix Epoch. Default: The current time is used.
        - nsecs - (Number between 0-9999) additional time, in 100-nanosecond units. Ignored if msecs is unspecified.
          Default: internal uuid counter is used, as per 4.2.1.2.
    :param buffer: Array or buffer where UUID bytes are to be written.
    :param offset: Starting index in buffer at which to begin writing.
	:returns: The buffer, if specified, otherwise the string form of the UUID.

    .. note::
        The randomly generated node id is only guaranteed to stay constant for the lifetime of the current JS
        runtime.


.. js:function:: uuid.v4([options [, buffer [, offset]]])

    Generate and return a RFC4122 v4 UUID.

    :param options: Optional object with uuid state to apply. Unused at the moment.
    :param buffer: Array or buffer where UUID bytes are to be written.
    :param offset: Starting index in buffer at which to begin writing.
    :returns: The buffer, if specified, otherwise the string form of the UUID.


.. js:function:: uuid.parse(id[, buffer[, offset]])
.. js:function:: uuid.unparse(buffer[, offset])

    Parse and unparse UUIDs.

    :param id: UUID(-like) string
    :param buffer: Array or buffer where UUID bytes are to be written. Default: A new Array or Buffer is used.
    :param offset: Starting index in buffer at which to begin writing. Default: 0.

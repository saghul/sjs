
.. _modcodecs:

codecs
======

The `codecs` module provides a consistent API for encoding (or objects) into different representations.

Currently ``json``, ``hex`` and ``base64`` are the supported encodings.


.. js:function:: encode(encoding, data)

    Encodes the given `data` using the desired `encoding`.

    :param encoding: The desired encoding. (Must be one of the supported encodings)
    :param data: Value which will be encoded.
    :returns: A string with the encoded data.

.. js:function:: decode(encoding, data)

    Decodes the given `data` using the desired `encoding`.

    :param encoding: The desired encoding. (Must be one of the supported encodings)
    :param data: Value which will be decoded.
    :returns: The decoded object.

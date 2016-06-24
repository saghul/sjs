
.. _modhash:

hash
====

This module provides facilities for creating varios hashes over data.


Hash objects
------------

All objects defined in this module inherit from a ``HashBase`` base class and thus have the same API.

.. js:class:: hash.HashBase

    Base class for all hash types. Currently the following classes are implemented:

    - ``MD5``

    - ``SHA1``

    - ``SHA256``

    - ``SHA512``

    .. js:function:: hash.HashBase.prototype.update(data)

        Update the hash object with the given data. This function can be called multiple times in order to incrementally
        update the data being hashed.

    .. js:function hash.HashBase.prototype.digest([encoding])

        Get the digest of the hashed data. If `encoding` is specified it must be one of ``hex`` or ``base64``, which
        returns the digest encoded as requested, otherwise a string is returned.

        This function can be called multiple times and even in between calls to ``update()``.


Functions
---------

.. js:function:: hash.createHash(type)

    Helper function to create hash objects. ``type`` must be (a string) one of:

    - md5

    - sha1

    - sha256

    - sha512


Examples
--------

Using the helper function and chaining methods:

::

	hash.createHash('sha1').update('abc').digest('hex');
	// "a9993e364706816aba3e25717850c26c9cd0d89d"

Creating objects and calling methods individually:

::

	var h1 = new hash.SHA1();
	h1.update('abc');
	h1.digest('hex');
	// "a9993e364706816aba3e25717850c26c9cd0d89d"


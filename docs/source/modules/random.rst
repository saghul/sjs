.. _modrandom:

random
======

This module provides random number generation functions.

The current random number generator is implemented as a
`Mersenne Twister <https://en.wikipedia.org/wiki/Mersenne_Twister>`_ PRNG which is
automatically seeded on startup with data from :js:func:`os.urandom`.

.. note::
    This module could use some peer review. If you are competent to do so, please get in touch.


Random objects
--------------

.. js:class:: random.Random([seed])

    Class implementing the Mersenne Twister PRNG. While users are free to create multiple instances
    of this class, the module exports the ``seed`` and ``random`` functions bound to a default
    instance which is seeded with the default seed.

.. js:function:: random.Random.prototype.seed([seed])

    Seed the PRNG. `seed` can be a number or an ``Array`` of numbers. If ``null`` or ``undefined`` is passed
    a default seed is obtained from :js:func:`os.urandom`. The default seed consists of 2496 bytes, enough to
    fill the Mersenne Twister state.

.. js:function:: random.Random.prototype.random

    Return a random floating point number in the [0.0, 1.0) interval.


SystemRandom objects
--------------------

.. js:class:: random.SystemRandom

    Class implementing a similar interface to :js:class:`random.Random`, but using :js:func:`os.urandom` as the source
    for random numbers.

.. js:function:: random.SystemRandom.prototype.random

    Return a random floating point number in the [0.0, 1.0) interval.


Functions
---------

.. js:function:: random.random

    Return a random floating point number in the [0.0, 1.0) interval. The default :js:class:`random.Random` instance
    is used.

.. js:function:: random.seed([seed])

    Seed the default :js:class:`random.Random` instance.

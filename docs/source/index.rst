
Skookum JS
==========

Skookum JS, or `sjs` for short, is a JavaScript runtime built on top of the `Duktape <http://duktape.org>`_ engine.
It provides a simple way to write applications using JavaScript with a traditional synchronous I/O model. It tries
to minimize abstractions and give the developer as much control as possible over the platform and low level APIs.


Quickstart
----------

::

    git clone https://github.com/saghul/sjs && cd sjs
    make && make run
    sjs> print('Hello JavaScript! \u2b22');
    Hello JavaScript! ⬢


Documentation
-------------

.. toctree::
    :maxdepth: 1

    features
    design
    usage
    modules
    faq


Building
--------

Compiling `sjs` is easy, the only dependency is the `CMake <https://cmake.org>`_ build system.

Compile
^^^^^^^

For Debug builds (the default):

::

    make

For Release builds:

::

    make BUILDTYPE=Release

The installation prefix can be specified by setting ``PREFIX``, it defaults to ``/usr/local``.

::

    make PREFIX=/usr

Install
^^^^^^^

::

    make install

By default `sjs` will be installed to the directory indicated by ``PREFIX`` (``/usr/local`` by default) with the
following structure:

* ``PREFIX``/bin: `sjs` binary
* ``PREFIX``/lib: `libsjs` library
* ``PREFIX``/lib/sjs/modules: modules

The destination of the files can be further altered by setting ``DESTDIR``. This will be prepended to ``PREFIX``.
Confusing, I know.

::

    make DESTDIR=/tmp/testsjs install

Run the test suite
^^^^^^^^^^^^^^^^^^

::

    make test

Run the CLI without installing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    make run

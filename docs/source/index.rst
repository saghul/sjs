
Skookum JS
==========

Skookum JS, or `sjs` for short, is a JavaScript runtime built on top of the `Duktape <http://duktape.org>`_ engine.
It provides a simple way to write applications using JavaScript with a traditional synchronous I/O model. It tries
to minimize abstractions and give the developer as much control as possible over the platform and low level APIs.


Quickstart
----------

.. code-block:: shell

    git clone https://github.com/saghul/sjs && cd sjs
    make && make run

.. raw:: html

    <script type="text/javascript" src="https://asciinema.org/a/dxn9vxjod2kvk2hi5il0tn8zd.js" id="asciicast-dxn9vxjod2kvk2hi5il0tn8zd" async></script>


Documentation
-------------

.. toctree::
    :maxdepth: 1

    features
    design
    usage
    module_system
    modules
    faq


Building
--------

Compiling `sjs` is easy, the only dependency is the `CMake <https://cmake.org>`_ build system.

Compile
^^^^^^^

For Debug builds (the default):

.. code-block:: shell

    make

For Release builds:

.. code-block:: shell

    make BUILDTYPE=Release

The installation prefix can be specified by setting ``PREFIX``, it defaults to ``/usr/local``.

.. code-block:: shell

    make PREFIX=/usr

Install
^^^^^^^

.. code-block:: shell

    make install

By default `sjs` will be installed to the directory indicated by ``PREFIX`` (``/usr/local`` by default) with the
following structure:

* ``PREFIX``/bin: `sjs` binary
* ``PREFIX``/lib: `libsjs` library
* ``PREFIX``/lib/sjs/modules: modules

The destination of the files can be further altered by setting ``DESTDIR``. This will be prepended to ``PREFIX``.
Confusing, I know.

.. code-block:: shell

    make DESTDIR=/tmp/testsjs install

Run the test suite
^^^^^^^^^^^^^^^^^^

.. code-block:: shell

    make test

Run the CLI without installing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: shell

    make run


.. _features:

Features
========

* Small footprint
* `Ecmascript ES5/ES5.1 <http://www.ecma-international.org/ecma-262/5.1/>`_ compliant
* Some post-ES5 features
* `TypedArray <https://www.khronos.org/registry/typedarray/specs/latest/>`_ and
  `Buffer <https://nodejs.org/docs/v0.12.1/api/buffer.html>`_ support
* Built-in regular expression engine
* Built-in Unicode support
* Tail call support
* Combined reference counting and mark-and-sweep garbage collection with finalization
* CommonJS-based module loading system
* Support for native modules written in C
* Rich standard library
* Binary name 25% shorter than Node

.. seealso::
    Skookum JS gets most of its features through its engine: `Duktape <http://duktape.org>`_.


Post-ES5 features
-----------------

Duktape (the JavaScript engine used by `sjs`) currently implements
`some post-ES5 features <http://wiki.duktape.org/PostEs5Features.html>`_.

`sjs` expands on this by having `babel-polyfill <https://babeljs.io/docs/usage/polyfill/>`_ builtin.

.. note::
    Some of the polyfills depend on core dunctionality not currently present, such as ``setTimeout``.


.. _features:

Features
========

* Small footprint
* `Ecmascript ES5/ES5.1 <http://www.ecma-international.org/ecma-262/5.1/>`_ compliant, some features borrowed
  from `Ecmascript ES6 <http://www.ecma-international.org/ecma-262/6.0/index.html>`_
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


ES6 support
-----------

Duktape (the JavaScript engine used by `sjs`) currently implements
`a subset of ES6 features <http://duktape.org/guide.html#es6features>`_.

`sjs` expands on this by provising `this ES6 shim <https://github.com/paulmillr/es6-shim/#es6-shim>`_ builtin.

.. note::
    No shim for `Promise` objects is currently provided.

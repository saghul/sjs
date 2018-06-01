
.. _design:

Design
======

Skookum JS is build by abstracting the undelying platform as little as possible and giving the developer full control
of low level POSIX APIs.

.. note::
    In release ``18.6.0`` SkookumJS went through a major internal redesign. The old design is still documented in this
    page, for posterity.

The SkookumJS VM is inspired by Node.js in the following ways:

* A single binary is provided, which contains all modules already builtin.
* Binary addons can be created, which can be linked with the ``sjs`` binary.

In addition, a ``libsjs`` library target is available for other applications to link with / embed.

Implementation details
----------------------

* The `Duktape <http://duktape.org/>`_ engine provides JavaScript capabilities.
* The `REPL (interactive mode) <https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop>`_ uses
  `linenoise <https://github.com/antirez/linenoise>`_.
* Modules can be either written in pure JavaScript or in C. See the :ref:`modules` section.

Retrospective
-------------

After first implementing the old design, I came to realization that it's more troublesome than the current one.
Specifically, these are issues that promted the redesign:

* Cumbersome fiddling with ``SJS_PATH`` to make sure the built binary and tests had access to modules.
* Complex build system with many shared libraries.
* Hard to have multiple versions side by side.
* Potential standard library / application mismatch.

In retrospect this should have been the design from the get go. It still offers the same amount of flexibility
(binary addons, `libsjs`) while making it easier to use and build.

------

Design (old)
------------

The VM is modeled after other runtimes such as Ruby's or Python's.

The runtime can be logically divided into 3 parts:

* The interpreter CLI: it's a relatively simple application which creates a `sjs` VM and evaluates code using it. For
  the `REPL (interactive mode) <https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop>`_ it uses
  `linenoise <https://github.com/antirez/linenoise>`_.
* The VM library: `sjs` (the CLI) links with `libsjs` (the VM), which encapsulates all functionality and embed the
  `Duktape <http://duktape.org/>`_ engine. This makes other applications capable of running JavaScript code by linking
  with `libsjs` and creating a VM.
* The modules: in `sjs` modules can be either written in pure JavaScript or in C. See the :ref:`modules` section.


.. image:: .static/sjs-libsjs-diagram.png
    :align: center


The VM
^^^^^^

The `sjs` VM is currently single threaded, bu thread aware. That is: users can create as many VMs as they wish, as long
as there is a single VM running for a given thread. There is no global state. There is no builtin mechanism for
inter-VM communication.

See the :ref:`vmapi` section for details on the C API.


Inspiration
^^^^^^^^^^^

Skookum JS was not inspired by browser JavaScript runtimes but by more "traditional" runtimes such as Ruby's MRI or
Python's CPython.

This means that the model is not inherently asynchronous and event-driven, as in a browser or
`Node JS <ihttps://nodejs.org/en/>`_. `sjs` does provide the necessary tools to build asynchronous / event-driven
frameworks on top of the provided modules.

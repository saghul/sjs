
.. _usage:

Usage
=====

Following its design, `sjs` can be used in 2 ways: the CLI and the C API.


CLI
---

This is the typical way to run JavaScript applications with `sjs`, by using the ``sjs`` binary.

::

    sjs -h
    Usage: sjs [options] [ <code> | <file> | - ]

    -h         show help text
    -i         enter interactive mode after executing argument file(s) / eval code
    -e CODE    evaluate code

    --use_strict    evaluate the code in strict mode

    If <file> is omitted, interactive mode is started automatically.

With the `sjs` CLI you can execute a file, eval some code directly from the command line, or enter the interactive
mode. By default strict mode is not used, unless ``--use_strict`` is used.

.. note::
    When using the CLI `sjs` will try to pretify the output of every command by serializing it using a JSON variant
    called `JX <http://duktape.org/guide.html#customjson.2>`_ which Duktape includes.

.. _vmapi:

Embedding sjs in your application
---------------------------------

The `sjs` VM API allows applications to execute JavaScript code using the `sjs` engine. The prime example is the `sjs`
CLI, see ``src/cli/main.c``.

With this C API applications willing to run JavaScript code can embed the `sjs` engine or link with `libsjs`.

Data types
^^^^^^^^^^

.. c:type:: sjs_vm_t

Opaque type encapsulating the `sjs` VM.

.. c:type:: duk_context

Opaque type encapsulating the Duktape engine.


API
^^^

**Main VM API**

.. c:function:: sjs_vm_t* sjs_vm_create(void)

    Create a `sjs` VM.

    :returns: The initialized VM.

.. c:function:: void sjs_vm_destroy(sjs_vm_t* vm)

    Destroy the given VM. It can no longer be used after calling this function.

    :param vm: The VM which will be destroyed.

.. c:function:: void sjs_vm_setup_args(sjs_vm_t* vm, int argc, char* argv[])

    Setup the VM's arguments. This is required for initializing some internals in the :ref:`modsystem` module.

    :param vm: The VM reference.
    :param argc: Number of arguments in the array.
    :param argv: Array with arguments, in comman-line form.

.. c:function:: duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm)

    Get the reference to the Duktape engine associated with the VM.

    :param vm: The VM reference.
    :returns: The Duktape context.

.. c:function:: int sjs_vm_eval_code(const sjs_vm_t* vm, const char* filename, const char* code, size_t len, FILE* foutput, FILE* ferror, bool use_strict)

    Evaluate the given JavaScript `code`.

    :param vm: The VM reference.
    :param filename: Indicates the filename that is being executed. It will be printed in tracebacks and such.
    :param code: What is going to be executed.
    :param len: Length of the code.
    :param foutput: Stream where to print the result of the evaulated code (can be NULL).
    :param ferror: Stream where to print errors, if any (can be NULL).
    :param use_strict: Indicates if the code should be evaluated in strict mode or not.
    :returns: 0 if the code was evaluated without errors, != 0 otherwise.

.. c:function:: int sjs_vm_eval_file(const sjs_vm_t* vm, const char* filename, FILE* foutput, FILE* ferror, bool use_strict)

    Evaluate the given file as JavaScript code.

    :param vm: The VM reference.
    :param filename: The file to be evaluated.
    :param foutput: Stream where to print the result of the evaulated code (can be NULL).
    :param ferror: Stream where to print errors, if any (can be NULL).
    :param use_strict: Indicates if the file should be evaluated in strict mode or not.
    :returns: 0 if the code was evaluated without errors, != 0 otherwise.

**Utility functions**

.. c:function:: int sjs_path_normalize(const char* path, char* normalized_path, size_t normalized_path_len)

    Normalize the given `path` into the given buffer. Mormalizing a path includes tilde expansions and ``realpath(3)``.

    :param path: The path which needs to be normalized.
    :param normalized_path: Buffer to store the normalized path.
    :param normalized_path_len: Size of `normalized_path`.
    :returns: 0 on success, or < 0 on failure. The returned code is the negated `errno`.

.. c:function:: uint64_t sjs_time_hrtime(void)

    Return the highest possible precission monotonic timer the system is able to provde.

    :returns: High precission time in nanoseconds.


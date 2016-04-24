
.. _vmapi:

VM API
======

The `sjs` VM API allows applications to execute JavaScript code using the `sjs` engine. The prime example is the `sjs`
CLI, see ``src/cli/main.c``.


Data types
----------

.. c:type:: sjs_vm_t

Opaque type encapsulating the `sjs` VM.

.. c:type:: duk_context

Opaque type encapsulating the Duktape engine.


API
---

Main VM API
^^^^^^^^^^^

.. c:function:: sjs_vm_t* sjs_vm_create(void)

    Create a `sjs` VM.

    :returns: The initialized VM.

.. c:function:: void sjs_vm_destroy(sjs_vm_t* vm)

    Destroy the given VM. It can no longer be used after calling this function.

    :param vm: The VM which will be destroyed.

.. c:function:: void sjs_vm_setup_args(sjs_vm_t* vm, int argc, char* argv[])

    Setup the VM's arguments. This is required for initializing some internals in the :ref:`mod_system` module.

    :param vm: The VM reference.
    :param argc: Number of arguments in the array.
    :param argv: Array with arguments, in comman-line form.

.. c:function:: duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm)

    Get the reference to the Duktape engine associated with the VM.

    :param vm: The VM reference.
    :returns: The Duktape context.

Utility functions
^^^^^^^^^^^^^^^^^

.. c:function:: int sjs_path_normalize(const char* path, char* normalized_path, size_t normalized_path_len)

    Normalize the given `path` into the given buffer. Mormalizing a path includes tilde expansions and ``realpath(3)``.

    :param path: The path which needs to be normalized.
    :param normalized_path: Buffer to store the normalized path.
    :param normalized_path_len: Size of `normalized_path`.
    :returns: 0 on success, or < 0 on failure. The returned code is the negated `errno`.

.. c:function:: uint64_t sjs_time_hrtime(void)

    Return the highest possible precission monotonic timer the system is able to provde.

    :returns: High precission time in nanoseconds.


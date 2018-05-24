
.. _modutils:

utils
=====

This package contains several modules grouping utility functions.


utils.object
------------

This object contains several utilities for working with objects. It oritinates from
`this module <https://github.com/defunctzombie/node-util>`_.


Functions
^^^^^^^^^

.. js:function:: utils.object.format(format, [...])

    Returns a formatted string using the first argument as a ``printf``-like format.

    :param format: A string that contains zero or more placeholders. Each placeholder is replaced with the converted
        value from its corresponding argument.
    :returns: The formatted string.

    Supported placeholders are:

        * %s - String.
        * %d - Number (both integer and float).
        * %j - JSON. Replaced with the string '[Circular]' if the argument contains circular references.
        * %% - single percent sign ('%'). This does not consume an argument.

    If the placeholder does not have a corresponding argument, the placeholder is not replaced.

    Examples:

    ::

        outils.format('%s:%s', 'foo'); // 'foo:%s'

    If there are more arguments than placeholders, the extra arguments are coerced to strings
    (for objects and symbols, :js:func:`utils.object.inspect` is used) and then concatenated, delimited by a space.

    ::

        outils.format('%s:%s', 'foo', 'bar', 'baz'); // 'foo:bar baz'

    If the first argument is not a format string then this function returns a string that is the
    concatenation of all its arguments separated by spaces. Each argument is converted to a string with
    :js:func:`utils.object.inspect`.

    ::

        outils.format(1, 2, 3); // '1 2 3'

.. js:function:: utils.object.inspect(object, [options])

    Return a string representation of `object`, which is useful for debugging.

    :param object: The entity to be inspected.
    :param options: Optional collection of settings which control how the formatted string is constructed.
    :returns: A formatted string useful for debugging.

    The `options` object alters certain aspects of the formatted string:

    * `showHidden` - if true then the object's non-enumerable and symbol properties will be shown too.
      Defaults to false.
    * `depth` - tells inspect how many times to recurse while formatting the object. This is useful for
      inspecting large complicated objects. Defaults to 2. To make it recurse indefinitely pass null.
    * `colors` - if true, then the output will be styled with ANSI color codes. Defaults to false. Colors are
      customizable, see below.
    * `customInspect` - if false, then custom inspect(depth, opts) functions defined on the objects being inspected
      won't be called. Defaults to true.

    Example of inspecting all properties of the util object:

    ::

        const outils = require('utils').object;
        print(outils.inspect(outil, { showHidden: true, depth: null }));

    Values may supply their own custom ``inspect(depth, opts)`` functions, when called they receive the current
    depth in the recursive inspection, as well as the options object passed to this function.

    Color output (if enabled) of this function is customizable globally via ``utils.object.inspect.styles`` and
    ``utils.object.inspect.colors`` objects.

    ``utils.object.inspect.styles`` is a map assigning each style a color from ``utils.object.inspect.colors``.
    Highlighted styles and their default values are: number (yellow) boolean (yellow) string (green) date (magenta)
    regexp (red) null (bold) undefined (grey) special - only function at this time (cyan) * name (intentionally no styling)

    Predefined color codes are: white, grey, black, blue, cyan, green, magenta, red and yellow.
    There are also bold, italic, underline and inverse codes.

    Objects also may define their own ``inspect(depth)`` function which this function will invoke and use the result
    of when inspecting the object:

    ::

        const outils = require('utils').object;

        var obj = { name: 'nate' };
        obj.inspect = function(depth) {
            return '{' + this.name + '}';
        };

        outils.inspect(obj);
        // "{nate}"

    You may also return another Object entirely, and the returned String will be formatted according to the returned
    Object. This is similar to how JSON.stringify() works:

    ::

        var obj = { foo: 'this will not show up in the inspect() output' };
        obj.inspect = function(depth) {
            return { bar: 'baz' };
        };

        outils.inspect(obj);
        // "{ bar: 'baz' }"

.. js:function:: utils.object.inherits(constructor, superConstructor)

    Inherit the prototype methods from one constructor into another. The prototype of `constructor` will be set to a
    new object created from `superConstructor`.

    As an additional convenience, `superConstructor` will be accessible through the ``constructor.super_`` property.

.. js:function:: utils.object.finalizer(object, funalizerFunc)

    Set or get the finalizer for the given `object`.

    :param object: Entity whose finalizer we are setting / getting.
    :param finalizerFunc: Function which will be called when the object is about to be freed.
    :returns: Undefined.

    .. seealso::
        `Duktape documentation on finalizers <http://duktape.org/guide.html#finalization>`_.


utils.unicode
-------------

This object provides unicode related utilities.


Functions
^^^^^^^^^

.. js:function:: utils.unicode.format(form, string)

    Normalize the given `string` to the requested unicode `form`. It uses the `unorm <https://github.com/walling/unorm>`_ module.

    :param form: Type of normalizatio to be applied. One of ``NFC``, ``NFD``, ``NFKC`` or ``NFKD``.
    :param string: Unicode string to be normalized.
    :returns: The normalized unicode string.


.. _modobjectutil:

objectutil
==========

This module contains several utilities for working with objects. It oritinates from
`this module <https://github.com/defunctzombie/node-util>`_.


Functions
---------

.. js:function:: objectutil.format(format, [...])

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

        objectutil.format('%s:%s', 'foo'); // 'foo:%s'

    If there are more arguments than placeholders, the extra arguments are coerced to strings
    (for objects and symbols, :js:func:`objectutil.inspect` is used) and then concatenated, delimited by a space.

    ::

        objectutil.format('%s:%s', 'foo', 'bar', 'baz'); // 'foo:bar baz'

    If the first argument is not a format string then this function returns a string that is the
    concatenation of all its arguments separated by spaces. Each argument is converted to a string with
    :js:func:`objectutil.inspect`.

    ::

        objectutil.format(1, 2, 3); // '1 2 3'

.. js:function:: objectutil.inspect(object, [options])

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

        const outil = require('objectutil');
        print(outil.inspect(outil, { showHidden: true, depth: null }));

    Values may supply their own custom ``inspect(depth, opts)`` functions, when called they receive the current
    depth in the recursive inspection, as well as the options object passed to this function.

    Color output (if enabled) of this function is customizable globally via ``objectutil.inspect.styles`` and
    ``objectutil.inspect.colors`` objects.

    ``objectutil.inspect.styles`` is a map assigning each style a color from ``objectutil.inspect.colors``.
    Highlighted styles and their default values are: number (yellow) boolean (yellow) string (green) date (magenta)
    regexp (red) null (bold) undefined (grey) special - only function at this time (cyan) * name (intentionally no styling)

    Predefined color codes are: white, grey, black, blue, cyan, green, magenta, red and yellow.
    There are also bold, italic, underline and inverse codes.

    Objects also may define their own ``inspect(depth)`` function which this function will invoke and use the result
    of when inspecting the object:

    ::

        const outil = require('objectutil');

        var obj = { name: 'nate' };
        obj.inspect = function(depth) {
            return '{' + this.name + '}';
        };

        outil.inspect(obj);
        // "{nate}"

    You may also return another Object entirely, and the returned String will be formatted according to the returned
    Object. This is similar to how JSON.stringify() works:

    ::

        var obj = { foo: 'this will not show up in the inspect() output' };
        obj.inspect = function(depth) {
            return { bar: 'baz' };
        };

        outil.inspect(obj);
        // "{ bar: 'baz' }"

.. js:function:: objectutil.inherits(constructor, superConstructor)

    Inherit the prototype methods from one constructor into another. The prototype of `constructor` will be set to a
    new object created from `superConstructor`.

    As an additional convenience, `superConstructor` will be accessible through the ``constructor.super_`` property.

.. js:function:: objectutil.finalizer(object, funalizerFunc)

    Set or get the finalizer for the given `object`.

    :param object: Entity whose finalizer we are setting / getting.
    :param finalizerFunc: Function which will be called when the object is about to be freed.
    :returns: Undefined.

    .. seealso::
        `Duktape documentation on finalizers <http://duktape.org/guide.html#finalization>`_.

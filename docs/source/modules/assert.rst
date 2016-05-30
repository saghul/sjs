
.. _modassert:

assert
======

This module is primarily used to write tests. It originates from
`this commonjs-assert module <https://github.com/defunctzombie/commonjs-assert>`_.


Functions
---------

.. js:function:: assert.fail(value, expected, message, operator)

    Throws an exception that displays the values for `value` and `expected` separated by the provided `operator`.
    If `message` is undefined, "value operator expected" is used.

.. js:function:: assert.ok(value, [message])

    Thests if `value` is truthy.

.. js:function:: assert.equal(value, expected, [message])

    Tests shallow, coercive equality with the equal comparison operator ( == ).

.. js:function:: assert.notEqual(value, expected, [message])

    Tests shallow, coercive non-equality with the not equal comparison operator ( != ).

.. js:function:: assert.deepEqual(value, expected, [message])

    Tests for deep equality.

.. js:function:: assert.notDeepEqual(value, expected, [message])

    Tests for any deep inequality.

.. js:function:: assert.strictEqual(value, expected, [message])

    Tests strict equality, as determined by the strict equality operator ( === ).

.. js:function:: assert.notStrictEqual(value, expected, [message])

    Tests strict non-equality, as determined by the strict not equal operator ( !== ).

.. js:function:: assert.throws(block, [error], [message])

    Expects `block` to throw an error. `error` can be constructor, RegExp or validation function.

    Validate instanceof using constructor:

    ::

        assert.throws(function() { throw new Error("Wrong value"); }, Error);

    Validate error message using RegExp:

    ::

        assert.throws(function() { throw new Error("Wrong value"); }, /value/);

    Custom error validation:

    ::

        assert.throws(function() {
            throw new Error("Wrong value");
        }, function(err) {
            if ( (err instanceof Error) && /value/.test(err) ) {
                return true;
            }
        }, "unexpected error");

.. js:function:: assert.doesNotThrow(block, [message])

    Expects `block` not to throw an error, see :js:func:`assert.throws` for details.

'use strict';

const assert = require('assert');
const select = require('io/select');


function badRfds() {
    return select.select({foo: 'bar'}, [], [], 0);
}

assert.throws(badRfds);

function badTimeout(t) {
    return select.select([], [], [], t);
}

var timeouts = [NaN, -3, {foo: 'bar'}, 'sjs'];
for (var i = 0, len = timeouts.length; i < len; i++) {
    assert.throws(badTimeout.bind(this, timeouts[i]));
}

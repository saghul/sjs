'use strict';

const assert = require('assert');
const poll = require('io/poll');


function badPfds() {
    return poll.poll({foo: 'bar'}, 0);
}

function badPfds2() {
    return poll.poll([{fd: 0, events: poll.POLLIN}, {foo: 'bar'}], 0);
}

assert.throws(badPfds);
assert.throws(badPfds2);

function badTimeout(t) {
    return poll.poll([], t);
}

var timeouts = [NaN, {foo: 'bar'}, [], 'sjs'];
for (var i = 0, len = timeouts.length; i < len; i++) {
    assert.throws(badTimeout.bind(this, timeouts[i]));
}

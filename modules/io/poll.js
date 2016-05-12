'use strict';

const _poll = require('_poll');


function poll(pfds, timeout) {
    if (timeout == null) {
        timeout = null;
    } else {
        timeout = parseFloat(timeout);
        if (isNaN(timeout) || timeout < 0) {
            throw new TypeError('timeout must be null or a positive number')
        }
    }
    return _poll.poll(pfds, timeout);
}


exports.poll = poll;

// extract constants
for (var k in _poll.c) {
    var val = _poll.c[k];
    exports[k] = val;
}

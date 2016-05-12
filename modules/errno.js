'use strict';

const _errno = require('_errno');


exports.strerror = _errno.strerror;
exports.map = new Map();

// extract constants
for (var k in _errno.c) {
    var val = _errno.c[k];
    exports[k] = val;
    exports.map.set(val, k);
}

'use strict';

const _errno = require('_errno');


module.exports.strerror = _errno.strerror;
module.exports.map = new Map();

// extract constants
for (var k in _errno.c) {
    var val = _errno.c[k];
    module.exports[k] = val;
    module.exports.map.set(val, k);
}

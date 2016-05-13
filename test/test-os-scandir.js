'use strict';

const assert = require('assert');
const errno = require('errno');
const os = require('os');


var r = os.scandir('.');
assert(r.length > 0);

try {
    os.scandir('hdu3gdb893dyu8o3ijddd');
} catch (e) {
    assert.equal(e.errno, errno.ENOENT)
}

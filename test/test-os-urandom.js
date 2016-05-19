'use strict';

const assert = require('assert');
const os = require('os');


var r;

r = os.urandom(64);
assert(r instanceof Buffer);
assert.equal(r.length, 64);

r = Buffer(128);
os.urandom(r);

var data = [null, NaN, 0, Buffer(0), new Uint8Array(16)];
for (var i = 0, len = data.length; i < len; i++) {
    assert.throws(os.urandom.bind(null, data[i]));
}

// test some large(r) value
r = os.urandom(4096);
assert(r instanceof Buffer);
assert.equal(r.length, 4096);

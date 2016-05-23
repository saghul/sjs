'use strict';

const assert = require('assert');
const random = require('random');


// some working seeds
var seeds = [0, 1, 1234, [0, 1, 2], null, undefined];
for (var i = 0; i < seeds.length; i++) {
    random.seed(seeds[i]);
}

// some non-working seeds
var seeds = [{}, NaN, Buffer(10), new Uint32Array(10)];
for (var i = 0; i < seeds.length; i++) {
    assert.throws(function() { random.seed(seeds[i]); });
}

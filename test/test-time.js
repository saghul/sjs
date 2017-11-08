'use strict';

const assert = require('assert');
const time = require('time');


var t = performance.now();
assert.ok(t > 0);

var t1 = performance.now();
time.sleep(0.5);
var diff = performance.now() - t1;
assert.ok(diff >= 500);


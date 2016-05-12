'use strict';

const assert = require('assert');
const time = require('time');


var t = time.time();
assert.ok(t > 0);

var t1 = time.hrtime();
time.sleep(0.5);
var diff = time.hrtime(t1);
assert.equal(diff[0], 0);
assert.ok(diff[1]/1000000 >= 500);



const assert = require('assert');
const time = require('time');


var t = time.time();
assert.ok(t > 0);

var t1 = time.hrtime();
time.sleep(1.1);
var diff = time.hrtime(t1);
assert.ok(diff[0] >= 1);


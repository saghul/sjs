'use strict';

const assert = require('assert');
const select = require('io/select');


var res = select.select([], [], [], 0);
assert.equal(res.rfds.length, 0);
assert.equal(res.wfds.length, 0);
assert.equal(res.xfds.length, 0);

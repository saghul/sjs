'use strict';

const assert = require('assert');
const poll = require('io/poll');


var res = poll.poll([], 0);
assert.equal(res.nevents, 0);
assert.equal(res.fds.length, 0);


'use strict';

const assert = require('assert');
const poll = require('io/poll');


var res = poll.poll([{fd: 1, events: poll.POLLOUT}], null);
assert.equal(res.nevents, 1);
assert.equal(res.fds.length, 1);
assert.equal(res.fds[0].fd, 1);
assert.equal(res.fds[0].revents, poll.POLLOUT);

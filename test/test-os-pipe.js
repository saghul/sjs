'use strict';

const assert = require('assert');
const os = require('os');

const TEST_DATA = 'hello sjs!';


var fds = os.pipe();
var rfd = fds[0];
var wfd = fds[1];

assert.equal(fds.length, 2);
assert.notEqual(rfd, -1);
assert.notEqual(wfd, -1);

os.write(wfd, TEST_DATA);
var data = os.read(rfd);
assert.equal(data, TEST_DATA);

os.close(rfd);
os.close(wfd);

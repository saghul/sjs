'use strict';

const assert = require('assert');
const os = require('os');

const TEST_FILE = 'test123';
const TEST_DATA = 'hello sjs!';


safeUnlink(TEST_FILE);

var fd, r;
fd = os.open(TEST_FILE, os.O_RDWR | os.O_CREAT | os.O_TRUNC);
assert(fd > -1);
r = os.write(fd, TEST_DATA);
assert.equal(r, TEST_DATA.length);
os.close(fd);

fd = os.open(TEST_FILE, 'r');
assert(fd > -1);
r = os.read(fd);
assert.equal(r, TEST_DATA);
os.close(fd);

safeUnlink(TEST_FILE);


function safeUnlink(path) {
    try {
        os.unlink(path);
    } catch (e) {
        // ignore
    }
}

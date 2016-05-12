'use strict';

const assert = require('assert');
const io = require('io');


io.stdout.write('lalala');
io.stdout.write(new Buffer('lalala'));
io.stderr.write('lalala');
io.stderr.write(new Buffer('lalala'));

assert.throws(function() {io.stdin.write('aaa');});
assert.throws(function() {io.stdout.read();});
assert.throws(function() {io.stderr.read();});

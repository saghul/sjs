'use strict';

const assert = require('assert');
const sys = require('system');


sys.stdout.write('lalala');
sys.stdout.write(new Buffer('lalala'));
sys.stderr.write('lalala');
sys.stderr.write(new Buffer('lalala'));

assert.throws(function() {sys.stdin.write('aaa');});
assert.throws(function() {sys.stdout.read();});
assert.throws(function() {sys.stderr.read();});

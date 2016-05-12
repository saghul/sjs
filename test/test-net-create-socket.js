'use strict';

const assert = require('assert');
const net = require('net');


var s1 = new net.Socket(net.AF_INET, net.SOCK_STREAM);
assert.ok(s1);
assert.notEqual(s1.fd, -1);
s1.close();
assert.equal(s1.fd, -1);

var s2 = new net.Socket(net.AF_INET, net.SOCK_STREAM);
assert.ok(s2);
assert.notEqual(s2.fd, -1);
s2.close();
assert.equal(s2.fd, -1);


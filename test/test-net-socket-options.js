'use strict';

const assert = require('assert');
const net = require('net');


// NB: this test does not work if the machine is Big Endian

var sock = new net.Socket(net.AF_INET, net.SOCK_STREAM);
var r;

// for getsockopt we set a boolen we get an int back
sock.setsockopt(net.SOL_SOCKET, net.SO_REUSEADDR, true);
r = sock.getsockopt(net.SOL_SOCKET, net.SO_REUSEADDR);
assert.equal(!!r, true);
sock.setsockopt(net.SOL_SOCKET, net.SO_REUSEADDR, false);
r = sock.getsockopt(net.SOL_SOCKET, net.SO_REUSEADDR);
assert.equal(!!r, false);

sock.setsockopt(net.SOL_SOCKET, net.SO_REUSEPORT, true);
sock.setsockopt(net.SOL_SOCKET, net.SO_REUSEPORT, false);

// this is how we set non-int options
var lingerOpts = new Buffer(8);
lingerOpts.writeInt32LE(1, 0);
lingerOpts.writeInt32LE(100, 4);
sock.setsockopt(net.SOL_SOCKET, net.SO_LINGER, lingerOpts.toString());

// for getting the options parse the returned string with a Buffer
r = sock.getsockopt(net.SOL_SOCKET, net.SO_LINGER, 8);
var resBuf = new Buffer(r);
assert.equal(resBuf.readInt32LE(0), 1)
assert.equal(resBuf.readInt32LE(4), 100)

// first reset the linger time
lingerOpts.fill(0);
lingerOpts.writeInt32LE(1, 0);
sock.setsockopt(net.SOL_SOCKET, net.SO_LINGER, lingerOpts.toString());

// now disable it (otherwise the time is remembered)
lingerOpts.fill(0);
sock.setsockopt(net.SOL_SOCKET, net.SO_LINGER, lingerOpts.toString());
r = sock.getsockopt(net.SOL_SOCKET, net.SO_LINGER, 8);
resBuf = new Buffer(r);
assert.equal(resBuf.readInt32LE(0), 0)
assert.equal(resBuf.readInt32LE(4), 0)

sock.close();

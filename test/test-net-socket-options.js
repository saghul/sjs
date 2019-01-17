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

// test SO_SNDTIMEO and SO_RCVTIMEO
var sndTimeout = new Buffer(16) // struct timeval is 16 bytes on 64-bit
sndTimeout.writeInt32LE(1, 0);  // tv_sec = 1
sndTimeout.writeInt32LE(0, 4);
sndTimeout.writeInt32LE(0, 8);  // tv_usec = 0
sndTimeout.writeInt32LE(0, 12);
sock.setsockopt(net.SOL_SOCKET, net.SO_SNDTIMEO, sndTimeout.toString());
r = sock.getsockopt(net.SOL_SOCKET,net.SO_SNDTIMEO,16);
resBuf = new Buffer(r);
assert.equal(resBuf.readInt32LE(0),  1);
assert.equal(resBuf.readInt32LE(4),  0);
assert.equal(resBuf.readInt32LE(8),  0);
assert.equal(resBuf.readInt32LE(12), 0);

var rcvTimeout = new Buffer(16) // struct timeval is 16 bytes on 64-bit
rcvTimeout.writeInt32LE(0, 0);     // tv_sec = 0
rcvTimeout.writeInt32LE(0, 4);
rcvTimeout.writeInt32LE(10000, 8); // tv_usec = 10000
rcvTimeout.writeInt32LE(0, 12);
console.log(rcvTimeout);
sock.setsockopt(net.SOL_SOCKET, net.SO_RCVTIMEO, rcvTimeout.toString());
r = sock.getsockopt(net.SOL_SOCKET,net.SO_RCVTIMEO,16);
resBuf = new Buffer(r);
// value of getsockopt for SO_RCVTIMEO is changed by the kernel
// for the set-value of 10000, the get-value is 12000 on my machine,
// but I can't guarantee whatever relationship those two numbers
// have will remain consistent from machine to machine, kernel to kernel,
// distro to distro, etc
// see [this StackOverflow post](https://stackoverflow.com/questions/42536919/why-so-rcvtimeo-timeout-differs-after-it-was-set)
// so, I just check that the value isn't too far off
var val,rx,ry
assert.equal(resBuf.readInt32LE(0),  0);
assert.equal(resBuf.readInt32LE(4),  0);
val = resBuf.readInt32LE(8);
rx = (val > (10000/2));
ry = (val < (10000*2));
assert.equal(rx,true);
assert.equal(ry,true);
assert.equal(resBuf.readInt32LE(12), 0);

sock.close();

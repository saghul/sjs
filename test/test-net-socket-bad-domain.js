'use strict';

const assert = require('assert');
const net = require('net');


function badSocketDomain() {
    var s = new net.Socket(1234, net.SOCK_STREAM);
}

assert.throws(badSocketDomain);


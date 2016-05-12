'use strict';

const assert = require('assert');
const net = require('net');

const TEST_DATA = 'PING';


var pair = net.socketpair(net.AF_UNIX, net.SOCK_STREAM);
pair[0].send(TEST_DATA);
var data = pair[1].recv();
assert.equal(data, TEST_DATA);
pair[1].send(TEST_DATA);
var data = pair[0].recv();
assert.equal(data, TEST_DATA);

pair[0].close();
pair[1].close();

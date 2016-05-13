'use strict';

const assert = require('assert');
const net = require('net');


const TEST_ADDR = {host: '127.0.0.1', port: 0};
const TEST_DATA = 'PING';


var server = new net.Socket(net.AF_INET, net.SOCK_DGRAM);
server.bind(TEST_ADDR);
TEST_ADDR.port = server.getsockname().port;

var client = new net.Socket(net.AF_INET, net.SOCK_DGRAM);
client.bind({host: '127.0.0.1', port: 0});

client.sendto(new Buffer(TEST_DATA), TEST_ADDR);
client.sendto(TEST_DATA, TEST_ADDR);
var buffer = new Buffer(4096);
var r = server.recvfrom(buffer);
assert.equal(buffer.slice(0, r.nrecv).toString(), TEST_DATA);
assert.equal(r.nrecv, TEST_DATA.length);
assert.deepEqual(r.address, client.getsockname());

server.close();
client.close();

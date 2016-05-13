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

client.sendto(TEST_DATA, TEST_ADDR);
var res = server.recvfrom();
assert.equal(res.data, TEST_DATA);
assert.deepEqual(res.address, client.getsockname());

server.close();
client.close();

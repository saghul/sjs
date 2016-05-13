'use strict';

const assert = require('assert');
const net = require('net');


const TEST_ADDR = {host: '127.0.0.1', port: 0};
const TEST_DATA = 'PING';

var server = new net.Socket(net.AF_INET, net.SOCK_STREAM);
server.bind(TEST_ADDR);
server.listen(128);
TEST_ADDR.port = server.getsockname().port;

var client = new net.Socket(net.AF_INET, net.SOCK_STREAM);
client.connect(TEST_ADDR);

var connection = server.accept();

connection.send(TEST_DATA);
var data = client.recv();
assert.equal(data, TEST_DATA);
client.send(TEST_DATA);
var data = connection.recv();
assert.equal(data, TEST_DATA);

server.close();
client.close();
connection.close();


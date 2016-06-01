'use strict';

const assert = require('assert');
const net = require('net');
const os = require('os');
const system = require('system');


if (system.platform !== 'linux') {
    // the test only works on Linux
    os.exit(0);
}


const TEST_ADDR = '\0sjs-sock';
const TEST_DATA = 'PING';

var server = new net.Socket(net.AF_UNIX, net.SOCK_STREAM);
server.bind(TEST_ADDR);
assert.equal(server.getsockname(), TEST_ADDR);
server.listen(128);

var client = new net.Socket(net.AF_UNIX, net.SOCK_STREAM);
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


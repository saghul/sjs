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

connection.send(new Buffer(TEST_DATA));
var buffer = new Buffer(4096);
var r = client.recv(buffer);
assert.equal(buffer.slice(0, r).toString(), TEST_DATA);
assert.equal(r, TEST_DATA.length);
client.send(TEST_DATA);
buffer.fill(0);
r = connection.recv(buffer);
assert.equal(buffer.slice(0, r).toString(), TEST_DATA);
assert.equal(r, TEST_DATA.length);

function recvData(data) {
    client.recv(data);
}
var data = ['aaaaa', {}, new Buffer(0), new Uint8Array(0)];
for (var i = 0, len = data.length; i < len; i++) {
    assert.throws(recvData.bind(this, data[i]), TypeError);
}

server.close();
client.close();
connection.close();


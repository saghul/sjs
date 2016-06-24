'use strict';

const assert = require('assert');
const hash = require('hash');


var md5_vectors = {
    'abc' : '900150983cd24fb0d6963f7d28e17f72',
    'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq' : '8215ef0796a20bcaaae116d3876c664a',
};

for (var k in md5_vectors) {
    var h = new hash.MD5();
    h.update(k);
    assert.equal(md5_vectors[k], h.digest('hex'));
}


var sha1_vectors = {
    'abc' : 'a9993e364706816aba3e25717850c26c9cd0d89d',
    'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq' : '84983e441c3bd26ebaae4aa1f95129e5e54670f1',
};

for (var k in sha1_vectors) {
    var h = new hash.SHA1();
    h.update(k);
    assert.equal(sha1_vectors[k], h.digest('hex'));
}


var sha256_vectors = {
    'abc' : 'ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad',
    'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq' : '248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1',
};

for (var k in sha256_vectors) {
    var h = new hash.SHA256();
    h.update(k);
    assert.equal(sha256_vectors[k], h.digest('hex'));
}


var sha512_vectors = {
    'abc' : 'ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f',
    'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq' : '204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c33596fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445',
};

for (var k in sha512_vectors) {
    var h = new hash.SHA512();
    h.update(k);
    assert.equal(sha512_vectors[k], h.digest('hex'));
}


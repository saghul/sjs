'use strict';

const assert = require('assert');
const hash = require('hash');


var h1 = hash.createHash('sha512').update('aaa').update('aaa');
var h2 = new hash.SHA512();
h2.update('aaaaaa');

assert.equal(h1.digest(), h2.digest());

'use strict';

const assert = require('assert');
const hash = require('hash');


var h1 = new hash.SHA1();
h1.update('aaaaaa');

var h2 = new hash.SHA1();
h2.update('aaa');
h2.digest();
h2.update('aaa');

assert.equal(h1.digest('hex'), h2.digest('hex'));

'use strict';

const assert = require('assert');
const system = require('system');


var endian = system.endianness;
assert.ok(endian === 'big' || endian === 'little');


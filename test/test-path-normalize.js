'use strict';

const assert = require('assert');
const path = require('path');


var p = path.normalize('~');
assert.notEqual(p, '~');

var bogusPath = '/this/path/wont/exist';
var p = path.normalize(bogusPath);
assert.equal(p, bogusPath);

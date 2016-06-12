'use strict';

const assert = require('assert');
const path = require('path');
const system = require('system');


system.path.unshift(path.dirname(__filename));
const shebangtest = require('./shebangtest');

assert.equal(shebangtest.foo(), 42);

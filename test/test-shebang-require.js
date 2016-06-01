'use strict';

const assert = require('assert');
const path = require('path');
const system = require('system');


system.path.unshift(path.dirname(__file__));
const shebangtest = require('./shebangtest');

assert.equal(shebangtest.foo(), 42);

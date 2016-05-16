'use strict';

const assert = require('assert');
const path = require('path');
system.path.unshift(path.dirname(__file__));
const shebangtest = require('./shebangtest');


assert.equal(shebangtest.foo(), 42);

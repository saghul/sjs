'use strict';

const assert = require('assert');
const path = require('path');
const system = require('system');


system.path.unshift(path.join(path.dirname(__file__), 'fixtures', 'require_broken_dir'));
const foo = require('foo');

assert.equal(foo.foo, 42);

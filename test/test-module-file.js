'use strict';

const assert = require('assert');
const path = require('path');

const THIS_FILE = 'test-module-file.js';


assert.equal(path.basename(__file__), THIS_FILE);

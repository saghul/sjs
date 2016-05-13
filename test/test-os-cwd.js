'use strict';

const assert = require('assert');
const os = require('os');


var cwd = os.cwd;
assert(typeof cwd === 'string');
print(cwd);

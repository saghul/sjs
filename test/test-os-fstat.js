'use strict';

const assert = require('assert');
const os = require('os');
const system = require('system');


const fd = os.open(system.executable, 'r');
const stat = os.fstat(fd);

assert(stat);
assert(stat.size);

os.close(fd);

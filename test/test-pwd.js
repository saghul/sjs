'use strict';

const assert = require('assert');
const pwd = require('pwd');
const os = require('os');


var uid = os.getuid();
var p = pwd.getpwuid(uid);
var p1 = pwd.getpwnam(p.name);
assert.deepEqual(p, p1);

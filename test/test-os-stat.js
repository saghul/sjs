'use strict';

const assert = require('assert');
const os = require('os');
const system = require('system');


var stat = os.stat(system.executable);
assert(stat);

assert.throws(function() { os.stat('/this/does/not/exist'); });

assert(os.S_ISREG(stat.mode));

const mode = os.S_IMODE(stat.mode);
assert.ok(mode === 0o0755 || mode === 0o0775);

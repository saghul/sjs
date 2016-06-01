'use strict';

const assert = require('assert');
const os = require('os');
const system = require('system');


var tmpDir;
if (system.platform === 'osx') {
    tmpDir = '/private/tmp';
} else {
    tmpDir = '/tmp';
}
var cwd = os.cwd;
os.chdir(tmpDir);
assert.equal(os.cwd, tmpDir);
os.chdir(cwd);
assert.equal(os.cwd, cwd);

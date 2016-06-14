'use strict';

const assert = require('assert');
const proc = require('process');
const sys = require('system');


var p;
var r;

p = proc.spawn([sys.executable, '-e', 'require(\'assert\')(require.main === undefined)'],
               {stdin: null, stdout: null, stderr: null});
r = p.wait();
assert.equal(r.exit_status, 0);
assert.equal(r.term_signal, 0);

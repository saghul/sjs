'use strict';

const assert = require('assert');
const path = require('path');
const proc = require('process');
const sys = require('system');


var p;
var r;

p = proc.spawn([sys.executable, path.join(__dirname, 'fixtures', 'require_main', 'foo.js')],
               {stdin: 'inherit', stdout: 'inherit', stderr: 'inherit'});
r = p.wait();
assert.equal(r.exit_status, 0);
assert.equal(r.term_signal, 0);

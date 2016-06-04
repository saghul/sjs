'use strict';

const assert = require('assert');
const proc = require('process');
const sys = require('system');

const TEXT = 'test123\n';
const TEST_ARGS = ['foo', 'bar', 'baz', '123'];


var p;
var r;

p = proc.spawn([sys.executable, 'test/helper3.js'],
               {stdin: 'pipe', stdout: 'pipe', stderr: 'pipe'});
p.stdin.write(TEXT);
assert.equal(p.stdout.readline(), TEXT);
assert.equal(p.stderr.readline(), TEXT);
r = p.wait();
assert.equal(r.exit_status, 0);
assert.equal(r.term_signal, 0);


p = proc.spawn([sys.executable, 'test/helper6.js'].concat(TEST_ARGS),
               {stdin: 'pipe', stdout: 'pipe', stderr: 'pipe'});
assert.deepEqual(JSON.parse(p.stdout.readline()), TEST_ARGS);
r = p.wait();
assert.equal(r.exit_status, 0);
assert.equal(r.term_signal, 0);

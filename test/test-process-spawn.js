'use strict';

const assert = require('assert');
const proc = require('process');


var p;
var r;

p = proc.spawn(['ls', '-l']);
r = p.wait();
assert.equal(r.exit_status, 0);
assert.equal(r.term_signal, 0);

p = proc.spawn('ls -l', {shell: true});
r = p.wait();
assert.equal(r.exit_status, 0);
assert.equal(r.term_signal, 0);

p = proc.spawn(['ls', '-l', '/bogus/path/1']);
r = p.wait();
assert.notEqual(r.exit_status, 0);
assert.equal(r.term_signal, 0);

p = proc.spawn('ls -l /bogus/path/1', {shell: true});
r = p.wait();
assert.notEqual(r.exit_status, 0);
assert.equal(r.term_signal, 0);



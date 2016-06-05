'use strict';

const assert = require('assert');
const errno = require('errno');
const os = require('os');
const proc = require('process');
const pwd = require('pwd');


var info = pwd.getpwnam('nobody');
if (info === null) {
    print('SKIP: there is no "nobody" user');
    os.exit(0);
}

var p;
var r;

try {
    p = proc.spawn(['ls', '-l'],
                   {cwd: '/', uid: info.uid, gid: info.gid});
} catch (e) {
    assert.equal(e.errno, errno.EPERM);
    print('SKIP: no permission to downgrade to user "nobody"');
    os.exit(0);
}
r = p.wait();
assert.equal(r.exit_status, 0);
assert.equal(r.term_signal, 0);

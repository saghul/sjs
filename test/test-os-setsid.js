'use strict';

const assert = require('assert');
const os = require('os');


os.setsid();
assert.throws(os.setsid);

var pid = os.fork();

if (pid == 0) {
    // child
    os.setsid();
} else {
    // parent
    var r = os.waitpid(pid, 0);
    assert.equal(r.pid, pid);
    assert(os.WIFEXITED(r.status));
    assert.equal(os.WEXITSTATUS(r.status), 0);
}

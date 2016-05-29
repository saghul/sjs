'use strict';

const assert = require('assert');
const os = require('os');

const parentPid = os.getpid();


var pid = os.fork();

if (pid == 0) {
    // child
    assert.equal(parentPid, os.getppid());
} else {
    // parent
    var r = os.waitpid(pid, 0);
    assert.equal(r.pid, pid);
    assert(os.WIFEXITED(r.status));
    assert.equal(os.WEXITSTATUS(r.status), 0);
}

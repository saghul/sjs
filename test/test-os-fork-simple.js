'use strict';

const assert = require('assert');
const os = require('os');

const sentinel = 4242;


var pid = os.fork();

if (pid == 0) {
    // child
    assert.equal(sentinel, 4242);
} else {
    // parent
    assert.equal(sentinel, 4242);
    var r = os.waitpid(pid, 0);
    assert.equal(r.pid, pid);
    assert(os.WIFEXITED(r.status));
    assert.equal(os.WEXITSTATUS(r.status), 0);
}

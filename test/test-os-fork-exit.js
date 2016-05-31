'use strict';

const assert = require('assert');
const os = require('os');


var pid = os.fork();

if (pid == 0) {
    // child
    os.exit(42);
} else {
    // parent
    var r = os.waitpid(pid);
    assert.equal(r.pid, pid);
    assert(os.WIFEXITED(r.status));
    assert.equal(os.WEXITSTATUS(r.status), 42);
}

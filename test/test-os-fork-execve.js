'use strict';

const assert = require('assert');
const os = require('os');
const path = require('path');
const system = require('system');


var p = os.pipe();
var rfd = p[0];
var wfd = p[1];
os.cloexec(wfd, false);
var pid = os.fork();

if (pid == 0) {
    // child
    os.execve(system.executable,
              [system.executable, path.join(__dirname, 'helper1.js')],
              {PARENT_FD: wfd});
    assert(false);
} else {
    // parent
    var data = os.read(rfd);
    assert.equal(data, "HELLO");
    var r = os.waitpid(pid);
    assert.equal(r.pid, pid);
    assert(os.WIFEXITED(r.status));
    assert.equal(os.WEXITSTATUS(r.status), 0);
}

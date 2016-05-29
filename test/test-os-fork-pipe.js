'use strict';

const assert = require('assert');
const os = require('os');

const sentinel = 4242;

var p = os.pipe();
var rfd = p[0];
var wfd = p[1];
var pid = os.fork();

if (pid == 0) {
    // child
    os.write(wfd, "HELLO");
} else {
    // parent
    var data = os.read(rfd);
    assert.equal(data, "HELLO");
    var r = os.waitpid(pid);
    assert.equal(r.pid, pid);
    assert(os.WIFEXITED(r.status));
    assert.equal(os.WEXITSTATUS(r.status), 0);
}

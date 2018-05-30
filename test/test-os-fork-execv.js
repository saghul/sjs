'use strict';

const assert = require('assert');
const os = require('os');
const path = require('path');
const system = require('system');


var p = os.pipe();
var rfd = p[0];
var wfd = p[1];


var pid = os.fork();

if (pid == 0) {
    // child
    os.dup2(wfd, 1, false);
    os.execv(system.executable,
              [system.executable, path.join(__dirname, 'helper8.js')]);
    assert(false);
} else {
    // parent
    os.close(wfd);
    var data = '';
    for (;;) {
        var chunk = os.read(rfd, 16834);
        chunk = new TextDecoder().decode(chunk);
        if (chunk) {
            data += chunk;
        } else {
            break;
        }
    }
    assert.equal(data, 'HELLO SJS\n');
    var r = os.waitpid(pid);
    assert.equal(r.pid, pid);
    assert(os.WIFEXITED(r.status));
    assert.equal(os.WEXITSTATUS(r.status), 0);
}

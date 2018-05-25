'use trict';

const assert = require('assert');
const io = require('io');
const os = require('os');
const path = require('path');
const system = require('system');

const TEXT = 'hello sjs!\n';


function popen(filename) {
    var stdin = os.pipe();
    var stdout = os.pipe();
    var stderr = os.pipe();

    var pid = os.fork();
    if (pid === 0) {
        // child
        os.dup2(stdin[0], 0, false);
        os.dup2(stdout[1], 1, false);
        os.dup2(stderr[1], 2, false);
        os.execv(system.executable, [system.executable, filename]);
        assert(false);
    } else {
        // parent
        os.close(stdin[0]);
        os.close(stdout[1]);
        os.close(stderr[1]);
        var r = {
            pid: pid,
            stdin_fd: stdin[1],
            stdout_fd: stdout[0],
            stderr_fd: stderr[0]
        };
        return r;
    }
}

var data;
var r = popen(path.join(__dirname, 'helper3.js'));
os.write(r.stdin_fd, TEXT);
data = os.read(r.stdout_fd);
assert.equal(data, TEXT);
data = os.read(r.stderr_fd);
assert.equal(data, TEXT);

os.close(r.stdin_fd);
os.close(r.stdout_fd);
os.close(r.stderr_fd);

var res = os.waitpid(r.pid);
assert.equal(r.pid, res.pid);
assert(os.WIFEXITED(res.status));
assert.equal(os.WEXITSTATUS(res.status), 0);

'use strict';

const assert = require('assert');
const errno = require('errno');
const io = require('io');
const os = require('os');


// usual double forking for Unix daemons

function daemonize() {
    var pid1 = os.fork();
    if (pid1 != 0) {
        // exit parent
        os._exit(0);
    }

    // become a session leader
    os.setsid();

    // change working directory
    os.chdir('/');

    // replace stdio
    var devNull = os.open('/dev/null', os.O_RDWR);
    os.dup2(devNull, 0, false);
    os.dup2(devNull, 1, false);
    os.dup2(devNull, 2, false);
    if (devNull > os.STDERR_FILENO) {
        os.close(devNull);
    }

    var pid2 = os.fork();
    if (pid2 != 0) {
        // exit parent
        os._exit(0);
    }
}


// spawn child processes

function Process(options) {
    this._pid = -1;
    this._options = options;
    this._stdin = null;
    this._stdout = null;
    this._stderr = null;
    this._result = null;

    Object.defineProperty(this, 'pid', {
        get: function() {
            return this._pid;
        }
    });

    Object.defineProperty(this, 'stdin', {
        get: function() {
            return this._stdin;
        }
    });

    Object.defineProperty(this, 'stdout', {
        get: function() {
            return this._stdout;
        }
    });

    Object.defineProperty(this, 'stderr', {
        get: function() {
            return this._stderr;
        }
    });

}


Process.prototype.spawn = function() {
    var options = this._options;

    // prepare stdio pipes
    //

    var devNull = -1;

    // pipe for communicating error back to the parent until execv/e
    var errorPipe = os.pipe();

    // these pipes represent the parent -> child communication direction
    var stdinPipe = [-1, -1];
    var stdoutPipe = [-1, -1];
    var stderrPipe = [-1, -1];

    if (options.stdin === null) {
        devNull = openDevNull();
        stdinPipe[0] = devNull;
    } else if (options.stdin === 'pipe') {
        stdinPipe = os.pipe();
    } else if (typeof options.stdin === 'number') {
        stdinPipe[0] = options.stdin;
        os.cloexec(stdinPipe[0], false);
    } else {
        // inherit
    }

    if (options.stdout === null) {
        if (devNull === -1) {
            devNull = openDevNull();
        }
        stdoutPipe[1] = devNull;
    } else if (options.stdout === 'pipe') {
        stdoutPipe = os.pipe();
    } else if (typeof options.stdout === 'number') {
        stdoutPipe[1] = options.stdout;
        os.cloexec(stdoutPipe[1], false);
    } else {
        // inherit
    }

    if (options.stderr === null) {
        if (devNull === -1) {
            devNull = openDevNull();
        }
        stderrPipe[1] = devNull;
    } else if (options.stderr === 'pipe') {
        stderrPipe = os.pipe();
    } else if (options.stderr === 'stdout') {
        if (stdoutPipe[1] !== -1) {
            stderrPipe[1] = stdoutPipe[1];
        } else {
            stderrPipe[1] = io.stdout.fd;
        }
    } else if (typeof options.stderr === 'number') {
        stderrPipe[1] = options.stderr;
        os.cloexec(stderrPipe[1], false);
    } else {
        // inherit
    }

    try {
        var pid = os.fork();
    } catch(e) {
        var fds = [].concat(errorPipe, stdinPipe, stdoutPipe, stderrPipe, devNull);
        fds.forEach(function(fd) {
            if (fd > os.STDERR_FILENO) {
                silentClose(fd);
            }
        });
        throw e;
    }

    if (pid === 0) {
        // child
        //
        try {
            if (stdinPipe[0] !== -1) {
                os.dup2(stdinPipe[0], 0, false);
            }
            if (stdoutPipe[1] !== -1) {
                os.dup2(stdoutPipe[1], 1, false);
            }
            if (stderrPipe[1] !== -1) {
                os.dup2(stderrPipe[1], 2, false);
            }

            if (options.cwd) {
                os.chdir(options.cwd)
            }

            os.setsid();

            if (options.uid !== null || options.gid !== null) {
                // Drop all supplementary groups. When dropping privileges from root some extra groups
                // may allow the process to act as root. Discard errors since this can fail if we are
                // not root.
                try {
                    os.setgroups([]);
                } catch (e) {
                }
            }

            if (options.gid !== null) {
                os.setgid(options.gid);
            }

            if (options.uid !== null) {
                os.setuid(options.uid);
            }

            if (options.env !== null) {
                os.execvpe(options.executable, options.args, options.env);
            } else {
                os.execvp(options.executable, options.args);
            }
        } catch (e) {
            os.write(errorPipe[1], String(e.errno));
        }
        os._exit(127);
    } else {
        // parent
        //

        // wait until child has started
        os.close(errorPipe[1]);
        var r = os.read(errorPipe[0]);
        if (r === '') {
            // child started succesfully
            r = 0;
        } else {
            var p = os.waitpid(pid, 0);
            assert.equal(p.pid, pid);
            var errorno = Number.parseInt(r);
            r = new Error('Faild to spawn child: [errno ' + errorno + '] ' + errno.strerror(errorno));
            r.errno = errorno;
        }

        // close fds
        var fds = [devNull, errorPipe[0], stdinPipe[0], stdoutPipe[1], stderrPipe[1]];
        fds.forEach(function(fd) {
            if (fd > os.STDERR_FILENO) {
                silentClose(fd);
            }
        });

        // throw error, if there was one
        if (r instanceof Error) {
            throw r;
        }

        // save pid
        this._pid = pid;

        // wrap child stdio in file-like objects
        if (stdinPipe[1] !== -1) {
            this._stdin = io.fdopen(stdinPipe[1], 'w', '<child stdin>', 0);
        }
        if (stdoutPipe[0] !== -1) {
            this._stdout = io.fdopen(stdoutPipe[0], 'r', '<child stdout>', 0);
        }
        if (stderrPipe[0] !== -1) {
            this._stderr = io.fdopen(stderrPipe[0], 'r', '<child stderr>', 0);
        }
    }
}


Process.prototype.wait = function() {
    if (this._result === null) {
        var r = os.waitpid(this._pid);
        assert.equal(r.pid, this._pid);
        this._result = {exit_status: 0, term_signal: 0};
        if (os.WIFEXITED(r.status)) {
            this._result.exit_status = os.WEXITSTATUS(r.status);
        }
        if (os.WIFSIGNALED(r.status)) {
            this._result.term_signal = os.WTERMSIG(r.status);
        }
    }
    return this._result;
}


const defaultOptions = {
    executable: null,
    args: null,
    env: null,
    stdin: 'inherit',
    stdout: 'inherit',
    stderr: 'inherit',
    shell: false,
    cwd: null,
    uid: null,
    gid: null
};

function spawn(cmd, options) {
    var procOptions = Object.assign({}, defaultOptions);
    options = options || {};

    procOptions.args = [];
    if (typeof cmd === 'string') {
        procOptions.args.push(cmd);
    } else if (Array.isArray(cmd)) {
        procOptions.args.push.apply(procOptions.args, cmd);
    } else {
        throw new TypeError('"cmd" must be a string on an array');
    }

    procOptions.shell = !!options.shell;
    if (procOptions.shell) {
        procOptions.args.unshift.apply(procOptions.args, ['/bin/sh', '-c']);
    }

    procOptions.executable = procOptions.args[0];

    if (options.env) {
        procOptions.env = Object.assign({}, options.env);
    }

    if (options.cwd) {
        procOptions.cwd = options.cwd;
    }

    procOptions.uid = options.uid || null;
    procOptions.gid = options.gid || null;

    procOptions.stdin = normalizeStdioOption(options.stdin, procOptions.stdin, ['inherit', 'pipe']);
    procOptions.stdout = normalizeStdioOption(options.stdout, procOptions.stdout, ['inherit', 'pipe']);
    procOptions.stderr = normalizeStdioOption(options.stderr, procOptions.stderr, ['inherit', 'pipe', 'stdout']);

    var proc = new Process(procOptions);
    proc.spawn();
    return proc;
}


// helpers

function normalizeStdioOption(option, defaultValue, validStrs) {
    if (option === undefined) {
        return defaultValue;
    } else if (option === null) {
        return option;
    } else if (typeof option === 'number') {
        var opt = option >>> 0;
        if (opt < 0 || Number.isNaN(opt)) {
            throw new Error('invalid option ' + option);
        }
        return opt;
    } else if (typeof option === 'string') {
        if (validStrs.indexOf(option) === -1) {
            throw new Error('invalid option ' + option);
        }
        return option;
    } else {
        throw new Error('invalid option ' + option);
    }
}


function openDevNull() {
    return os.open('/dev/null', os.O_RDWR);
}


function silentClose(fd) {
    assert(fd !== -1);
    try {
        os.close(fd);
    } catch (e) {
        // ignore
    }
}


exports.daemonize = daemonize;
exports.spawn = spawn;

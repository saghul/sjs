'use strict';

const _os = require('_os');


const O_APPEND = _os.c.O_APPEND || 0;
const O_CREAT  = _os.c.O_CREAT || 0;
const O_EXCL   = _os.c.O_EXCL || 0;
const O_RDONLY = _os.c.O_RDONLY || 0;
const O_RDWR   = _os.c.O_RDWR || 0;
const O_SYNC   = _os.c.O_SYNC || 0;
const O_TRUNC  = _os.c.O_TRUNC || 0;
const O_WRONLY = _os.c.O_WRONLY || 0;


function open(path, flags, mode) {
    if (typeof flags === 'string') {
        flags = stringToFlags(flags);
    }
    mode = modeNum(mode, '0666');
    return _os.open(path, flags, mode >>> 0);
}


function read(fd, nread) {
    if (nread === undefined || typeof nread === 'number') {
        nread = (nread >>> 0) || 4096;
    }
    // we will validate if the passed argument is a buffer in C
    return _os.read(fd, nread);
}


Object.defineProperty(exports, 'cwd', {
    get: function() {
        return _os.getcwd();
    }
});


function urandom(bytes) {
    if (typeof bytes === 'number') {
        if (bytes <= 0 || Number.isNaN(bytes)) {
            throw new RangeError('"bytes" must be > 0');
        }
        var buf = Buffer(bytes);
    } else if (bytes instanceof Buffer) {
        var buf = bytes;
        if (buf.length === 0) {
            throw new RangeError('buffer length must be > 0');
        }
    } else {
        throw new TypeError('only numbers and Buffer objects are allowed');
    }
    _os.urandom(buf);
    return buf;
}


function buildStat(
        dev,
        mode,
        nlink,
        uid,
        gid,
        rdev,
        ino,
        size,
        blksize,
        blocks,
        flags,
        gen,
        atim_msec,
        mtim_msec,
        ctim_msec,
        birthtim_msec) {
    this.dev = dev;
    this.mode = mode;
    this.nlink = nlink;
    this.uid = uid;
    this.gid = gid;
    this.rdev = rdev;
    this.ino = ino;
    this.size = size;
    this.blksize = blksize;
    this.blocks = blocks;
    this.flags = flags;
    this.gen = gen;
    this.atime = new Date(atim_msec);
    this.mtime = new Date(mtim_msec);
    this.ctime = new Date(ctim_msec);
    this.birthtime = new Date(birthtim_msec);

    return this;
}

function stat(path) {
    return _os.stat(path, buildStat);
}

// stat things

function S_IMODE(mode) {
    return mode & 4095; /* 07777 */
}

function S_IFMT(mode) {
    return mode & 61440; /* 0170000 */
}

// Constants used as S_IFMT() for various file types
// (not all are implemented on all systems)

// Functions to test for each file type

function S_ISDIR(mode) {
    return S_IFMT(mode) === _os.c.S_IFDIR;
}

function S_ISCHR(mode) {
    return S_IFMT(mode) === _os.c.S_IFCHR;
}

function S_ISBLK(mode) {
    return S_IFMT(mode) === _os.c.S_IFBLK;
}

function S_ISREG(mode) {
    return S_IFMT(mode) === _os.c.S_IFREG;
}

function S_ISFIFO(mode) {
    return S_IFMT(mode) === _os.c.S_IFIFO;
}

function S_ISLNK(mode) {
    return S_IFMT(mode) === _os.c.S_IFLNK;
}

function S_ISSOCK(mode) {
    return S_IFMT(mode) === _os.c.S_IFSOCK;
}


function execve(filename, argv, envp) {
    // normalize argv
    if (argv == null) {
        argv = [filename];
    } else if (!Array.isArray(argv)) {
        throw new Error('"argv" must be null, undefined or an Array');
    }
    if (envp == null) {
        envp = [];
    } else {
        var envArray = [];
        for (var prop in envp) {
            envArray.push(prop + '=' + envp[prop]);
        }
        envp = envArray;
    }
    _os.execve(filename, argv, envp);
}


function waitpid(pid, options) {
    return _os.waitpid(pid, options >>> 0);
}


function dup2(oldfd, newfd, cloexec) {
    if (cloexec == null) {
        cloexec = true;
    }
    return _os.dup2(oldfd, newfd, !!cloexec);
}


function exit(status) {
    // TODO: properly tear down the vm
    _os.exit(status >>> 0);
}


// internal helpers

function stringToFlags(flag) {
    switch (flag) {
        case 'r' : return O_RDONLY;
        case 'rs' : // fall through
        case 'sr' : return O_RDONLY | O_SYNC;
        case 'r+' : return O_RDWR;
        case 'rs+' : // fall through
        case 'sr+' : return O_RDWR | O_SYNC;

        case 'w' : return O_TRUNC | O_CREAT | O_WRONLY;
        case 'wx' : // fall through
        case 'xw' : return O_TRUNC | O_CREAT | O_WRONLY | O_EXCL;

        case 'w+' : return O_TRUNC | O_CREAT | O_RDWR;
        case 'wx+': // fall through
        case 'xw+': return O_TRUNC | O_CREAT | O_RDWR | O_EXCL;

        case 'a' : return O_APPEND | O_CREAT | O_WRONLY;
        case 'ax' : // fall through
        case 'xa' : return O_APPEND | O_CREAT | O_WRONLY | O_EXCL;

        case 'a+' : return O_APPEND | O_CREAT | O_RDWR;
        case 'ax+': // fall through
        case 'xa+': return O_APPEND | O_CREAT | O_RDWR | O_EXCL;
    }

    throw new Error('Unknown file open flag: ' + flag);
}


function modeNum(m, def) {
    if (typeof m === 'number')
        return m;
    if (typeof m === 'string')
        return parseInt(m, 8);
    if (def)
        return modeNum(def);
    return 0;
}


exports.abort    = _os.abort;
exports.open     = open;
exports.read     = read;
exports.write    = _os.write;
exports.close    = _os.close;
exports.pipe     = _os.pipe;
exports.isatty   = _os.isatty;
exports.ttyname  = _os.ttyname;
exports.scandir  = _os.scandir;
exports.stat     = stat;
exports.unlink   = _os.unlink;
exports.urandom  = urandom;
exports.getpid   = _os.getpid;
exports.getppid  = _os.getppid;
exports.dup      = _os.dup;
exports.dup2     = dup2;
exports.chdir    = _os.chdir;
exports.exit     = exit;
exports._exit    = _os._exit;
exports.setsid   = _os.setsid;

exports.S_IMODE  = S_IMODE;
exports.S_ISDIR  = S_ISDIR;
exports.S_ISCHR  = S_ISCHR;
exports.S_ISBLK  = S_ISBLK;
exports.S_ISREG  = S_ISREG;
exports.S_ISFIFO = S_ISFIFO;
exports.S_ISLNK  = S_ISLNK;
exports.S_ISSOCK = S_ISSOCK;

exports.fork         = _os.fork;
exports.execve       = execve;
exports.waitpid      = waitpid;
exports.WIFEXITED    = _os.WIFEXITED;
exports.WEXITSTATUS  = _os.WEXITSTATUS;
exports.WIFSIGNALED  = _os.WIFSIGNALED;
exports.WTERMSIG     = _os.WTERMSIG;
exports.WIFSTOPPED   = _os.WIFSTOPPED;
exports.WSTOPSIG     = _os.WSTOPSIG;
exports.WIFCONTINUED = _os.WIFCONTINUED;

exports.cloexec  = _os.cloexec;
exports.nonblock = _os.nonblock;


// extract constants
for (var k in _os.c) {
    var val = _os.c[k];
    exports[k] = val;
}

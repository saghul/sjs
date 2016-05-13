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


exports.abort = _os.abort;
exports.open  = open;
exports.read  = read;
exports.write = _os.write;
exports.close = _os.close;
exports.pipe  = _os.pipe;
exports.isatty = _os.isatty;
exports.ttyname = _os.ttyname;

// extract constants
for (var k in _os.c) {
    var val = _os.c[k];
    exports[k] = val;
}

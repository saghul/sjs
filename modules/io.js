'use strict';

const _io = _system.binding('io');
const _select = require('_io_select');
const _poll = require('_io_poll');

const errno = require('errno');
const os = require('os');
const path = require('path');
const outil = require('utils').object;


function File(path, f, mode, buffering) {
    this._path = path;
    this._f = f;
    this._mode = mode;

    if (buffering !== undefined) {
        switch (buffering) {
            case -1:
                // default, do nothing
                break;
            case 0:
            case 1:
                // 0 is unbuffered, 1 is line-buffered
                _io.setvbuf(this._f, buffering);
                break;
            default:
                this.close();
                throw new RangeError('invalid value for "buffering": ' + buffering);
        }
    }

    outil.finalizer(this, fileDealloc);

    Object.defineProperty(this, 'path', {
        get: function() {
            return this._path;
        }
    });

    Object.defineProperty(this, 'fd', {
        get: function() {
            if (this._f === null) {
                return -1;
            } else {
                return _io.fileno(this._f);
            }
        }
    });

    Object.defineProperty(this, 'mode', {
        get: function() {
            return this._mode;
        }
    });

    Object.defineProperty(this, 'closed', {
        get: function() {
            return this._f === null;
        }
    });
}


File.prototype.read = function(nread) {
    checkFile.call(this);
    if (nread === undefined || typeof nread === 'number') {
        nread = (nread >>> 0) || 4096;
    }
    // we will validate if the passed argument is a buffer in C
    return _io.fread(this._f, nread);
}


File.prototype.readLine = function(nread) {
    checkFile.call(this);
    if (nread === undefined || typeof nread === 'number') {
        nread = (nread >>> 0) || 4096;
    }
    // we will validate if the passed argument is a buffer in C
    return _io.fgets(this._f, nread);
}


File.prototype.write = function(data) {
    checkFile.call(this);
    return _io.fwrite(this._f, data);
}


File.prototype.writeLine = function(data) {
    checkFile.call(this);
    return _io.fwrite(this._f, data) + _io.fwrite(this._f, '\n');
}


File.prototype.flush = function() {
    checkFile.call(this);
    _io.fflush(this._f);
}


File.prototype.close = function() {
    if (this._f !== null) {
        _io.fclose(this._f);
        this._f = null;
    }
}


function open(fpath, mode, buffering) {
    const npath = path.normalize(fpath);
    const f = _io.fopen(npath, mode);

    return new File(npath, f, mode, buffering);
}


function fdopen(fd, mode, fpath, buffering) {
    const npath = path.normalize(fpath);
    const f = _io.fdopen(fd, mode);

    return new File(npath, f, mode, buffering);
}


function readFile(fpath) {
    const f = open(fpath, 'rb');
    const st = os.fstat(f.fd);
    const data = f.read(st.size);

    f.close();

    return data;
}

// internal helpers

function fileDealloc(f) {
    if (f._f !== null) {
        _io.fclose(f._f);
    }
}


function checkFile() {
    if (this._f === null) {
        const e = new Error('[Errno ' + errno.EBADF + '] ' + errno.strerror(errno.EBADF));
        e.errno = errno.EBADF;
        throw e;
    }
}


// exports

exports.open     = open;
exports.fdopen   = fdopen;
exports.readFile = readFile;
exports.select   = _select;
exports.poll     = _poll;

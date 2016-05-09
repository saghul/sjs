
const errno = require('errno');
const _fs = require('_fs');
const outil = require('objectutil');


function File(path, f, mode) {
    this._path = path;
    this._f = f;
    this._mode = mode;

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
                return _fs.fileno(this._f);
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
    return _fs.fread(this._f, nread);
}


File.prototype.write = function(data) {
    checkFile.call(this);
    return _fs.fwrite(this._f, data);
}


File.prototype.flush = function() {
    checkFile.call(this);
    _fs.fflush(this._f);
}


File.prototype.close = function() {
    if (this._f !== null) {
        _fs.fclose(this._f);
        this._f = null;
    }
}


function open(path, mode) {
    var f = _fs.fopen(path, mode);
    return new File(path, f, mode);
}


function fdopen(fd, mode, path) {
    var f = _fs.fdopen(fd, mode);
    return new File(path, f, mode);
}


// internal helpers

function fileDealloc(f) {
    if (f._f !== null) {
        _fs.close(f._f);
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

exports.File   = File;
exports.open   = open;
exports.fdopen = fdopen;
exports.unlink = _fs.unlink;

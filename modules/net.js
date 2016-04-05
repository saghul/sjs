'use strict';

const errno = require('errno');
const _gai = require('_gai');
const _socket = require('_socket');


function Socket(domain, type, protocol, fd) {
    var protocol = protocol >>> 0;

    if (fd && fd > -1) {
        this._fd = fd;
    } else {
        this._fd = _socket.socket(domain, type, protocol);
    }
    this._domain = domain;
    this._type = type;
    this._proto = protocol;

    // set finalizer
    Duktape.fin(this, socketDealloc);

    Object.defineProperty(this, 'fd', {
        get: function() {
            return this._fd;
        }
    });
}


Socket.prototype.accept = function() {
    checkSocket.call(this);
    var fd = _socket.accept(this._fd);
    return new Socket(this._domain, this._type, this._proto, fd);
}


Socket.prototype.bind = function(address) {
    checkSocket.call(this);
    _socket.bind(this._fd, this._domain, normalizeAddress(this._domain, address));
}


Socket.prototype.close = function() {
    if (this._fd !== -1) {
        _socket.close(this._fd);
        this._fd = -1;
    }
}


Socket.prototype.connect = function(address) {
    checkSocket.call(this);
    _socket.connect(this._fd, this._domain, normalizeAddress(this._domain, address));
}


Socket.prototype.getsockname = function() {
    checkSocket.call(this);
    return _socket.getsockname(this._fd);
}


Socket.prototype.getpeername = function() {
    checkSocket.call(this);
    return _socket.getpeername(this._fd);
}


Socket.prototype.listen = function(backlog) {
    checkSocket.call(this);
    var backlog = backlog >>> 0;
    _socket.listen(this._fd, backlog);
}


Socket.prototype.read = function(nread) {
    checkSocket.call(this);
    var nread = (nread >>> 0) || 4096;
    return _socket.read(this._fd, nread);
}


Socket.prototype.write = function(data) {
    checkSocket.call(this);
    return _socket.write(this._fd, data);
}


Socket.prototype.recvfrom = function(nread) {
    checkSocket.call(this);
    var nread = (nread >>> 0) || 4096;
    return _socket.recvfrom(this._fd, nread);
}


Socket.prototype.sendto = function(data, address) {
    checkSocket.call(this);
    return _socket.sendto(this._fd, this._domain, data, normalizeAddress(this._domain, address));
}


Socket.prototype.shutdown = function(how) {
    checkSocket.call(this);
    var how = how >>> 0;
    _socket.shutdown(this._fd, how);
}


// finalizer: will get called when a Socket is garbage collected
function socketDealloc(sock) {
    if (sock._fd !== -1) {
        _socket.close(sock._fd);
    }
}


function checkSocket() {
    if (this._fd === -1) {
        const e = new Error('[Errno ' + errno.EBADF + '] ' + errno.strerror(errno.EBADF));
        e.errno = errno.EBADF;
        throw e;
    }
}


function normalizeAddress(domain, address) {
    var addr = address || {};
    switch (domain) {
        case _socket.c.AF_INET:
            addr.host = addr.host || '0.0.0.0';
            addr.port = addr.port >>> 0;
            break;
        case _socket.c.AF_INET6:
            addr.host = addr.host || '::';
            addr.port = addr.port >>> 0;
            addr.flowinfo = addr.flowinfo >>> 0;
            addr.scopeid = addr.scopeid >>> 0;
            break;
        case _socket.c.AF_UNIX:
            // nothing
            break;
        default:
            throw new Error('invalid socket domain');
    }
    return addr;
}


function getaddrinfo(hostname, servname, hints) {
    var hints = hints || {};
    return _gai.getaddrinfo(hostname, servname, hints);
}


module.exports = {
    Socket        : Socket,
    getaddrinfo   : getaddrinfo,
    gai_strerror  : _gai.gai_strerror,
    gai_error_map : new Map()
};

// extract constants
for (var k in _gai.c) {
    var val = _gai.c[k];
    module.exports[k] = val;
    module.exports.gai_error_map.set(val, k);
}
for (var k in _socket.c) {
    module.exports[k] = _socket.c[k];
}

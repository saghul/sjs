'use strict';

const _hash  = _system.binding('hash');

const codecs = require('codecs');
const outil  = require('utils').object;


function HashBase() {
    this._ctx = this._init();

    // set finalizer
    outil.finalizer(this, hashDealloc);
}

HashBase.prototype.update = function(data) {
    this._update(data);
    return this;
}

HashBase.prototype.digest = function(encoding) {
    var digest = this._digest();
    if (encoding === 'hex' || encoding === 'base64') {
        return codecs.encode(encoding, digest);
    } if (encoding === void 0) {
        return digest;
    } else {
        throw new RangeError('unsupported encoding: ' + encoding);
    }
}


function MD5() {
    HashBase.call(this);
}

outil.inherits(MD5, HashBase);

MD5.prototype._init = function() {
    return _hash.md5_new();
}

MD5.prototype._update = function(data) {
    _hash.md5_update(this._ctx, data);
}

MD5.prototype._digest = function() {
    return _hash.md5_digest(this._ctx);
}


function SHA1() {
    HashBase.call(this);
}

outil.inherits(SHA1, HashBase);

SHA1.prototype._init = function() {
    return _hash.sha1_new();
}

SHA1.prototype._update = function(data) {
    _hash.sha1_update(this._ctx, data);
}

SHA1.prototype._digest = function() {
    return _hash.sha1_digest(this._ctx);
}


function SHA256() {
    HashBase.call(this);
}

outil.inherits(SHA256, HashBase);

SHA256.prototype._init = function() {
    return _hash.sha256_new();
}

SHA256.prototype._update = function(data) {
    _hash.sha256_update(this._ctx, data);
}

SHA256.prototype._digest = function() {
    return _hash.sha256_digest(this._ctx);
}


function SHA512() {
    HashBase.call(this);
}

outil.inherits(SHA512, HashBase);

SHA512.prototype._init = function() {
    return _hash.sha512_new();
}

SHA512.prototype._update = function(data) {
    _hash.sha512_update(this._ctx, data);
}

SHA512.prototype._digest = function() {
    return _hash.sha512_digest(this._ctx);
}


function createHash(type) {
    switch (type) {
        case 'md5':
            return new MD5();
        case 'sha1':
            return new SHA1();
        case 'sha256':
            return new SHA256();
        case 'sha512':
            return new SHA512();
        default:
            throw new RangeError('invalid hash type: ' + type);
    }
}


// helpers

function hashDealloc(hash) {
    _hash.dealloc(hash._ctx);
    hash._ctx = null;
}


// exports

exports.createHash = createHash;
exports.MD5        = MD5;
exports.SHA1       = SHA1;
exports.SHA256     = SHA256;
exports.SHA512     = SHA512;

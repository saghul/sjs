'use strict';

const _random = require('_random');
const codecs = require('codecs');
const os = require('os');
const outil = require('objectutil');


function Random() {
    this._state = _random.alloc();

    // set finalizer
    outil.finalizer(this, randomDealloc);

    // initial seed
    this.seed();
}


Random.prototype.seed = function(seed) {
    if (seed == null) {
        var seedBuf = os.urandom(2496);    // 624 * 4, to cover the entire MT state
        var seedBufLen = seedBuf.length;
        seed = [];
        for (var i = 0; i < seedBufLen; i += 4) {
            seed.push(seedBuf.readUInt32LE(i));
        }
    } else if ((typeof seed !== 'number' || Number.isNaN(seed)) && !Array.isArray(seed)) {
        throw new Error('seed may only be called with null, undefined, a number or an array of integers');
    }
    _random.seed(this._state, seed);
}


Random.prototype.random = function() {
    return _random.random(this._state);
}


function SystemRandom() {
}


SystemRandom.prototype.random = function() {
    var num = uint53();
    return num / 0x20000000000000;
}


// - helpers

// get a 53 bit unsigned int from the system random source
function uint53() {
    var high = os.urandom(4).readUInt32LE() & 0x1fffff;
    var low = os.urandom(4).readUInt32LE() >>> 0;
    return (high * 0x100000000) + low;
}

// the default Random object
const default_random = new Random();

// finalizer: will get called when a Random instance is garbage collected
function randomDealloc(random) {
    if (random._state !== null) {
        _random.dealloc(random.state);
        random._state = null;
    }
}


exports.Random = Random;
exports.random = Random.prototype.random.bind(default_random);
exports.seed   = Random.prototype.seed.bind(default_random);
exports.SystemRandom = SystemRandom;

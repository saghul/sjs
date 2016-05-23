'use strict';

const assert = require('assert');
const random = require('random');

const N = 5000;

var sources = [new random.Random(), new random.SystemRandom()];
for (var j = 0; j < sources.length; j++) {
    var src = sources[j];
    for (var i = 0; i < N; i++) {
        var n = src.random();
        assert(n >= 0.0 && n < 1.0);
    }
}

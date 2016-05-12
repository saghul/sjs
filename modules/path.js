'use strict';

const _path = require('_path');


function join() {
    if (arguments.length === 0)
        return '.';
    var joined;
    for (var i = 0; i < arguments.length; ++i) {
        var arg = arguments[i];
        if (arg.length > 0) {
            if (joined === undefined) {
                joined = arg;
            } else {
                var sep = '';
                if (arg.startsWith('/')) {
                    arg = arg.substring(1);
                }
                if (!joined.endsWith('/')) {
                    sep = '/';
                }
                joined += sep + arg;
            }
        }
    }
    if (joined === undefined)
        return '.';
    return joined;
}


function normalize(p) {
    try {
        return _path.normalize(p);
    } catch (e) {
        return p;
    }
}


exports.basename  = _path.basename;
exports.dirname   = _path.dirname;
exports.normalize = normalize;
exports.join      = join;

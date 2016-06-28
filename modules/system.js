'use strict';

const io = require('io');
const os = require('os');
// _system is a builtin


const staticProps = ['arch', 'endianness', 'executable', 'platform'];

for (var i = 0; i < staticProps.length; i++) {
    var prop = staticProps[i];
    Object.defineProperty(exports, prop, {
        configurable: false,
        enumerable: true,
        writable: false,
        value: _system[prop]
    });
}


const frozenProps = ['build', 'env', 'versions'];

for (var i = 0; i < frozenProps.length; i++) {
    var prop = frozenProps[i];
    Object.defineProperty(exports, prop, {
        configurable: false,
        enumerable: true,
        writable: false,
        value: Object.freeze(_system[prop])
    });
}


Object.defineProperty(exports, 'path', {
    configurable: false,
    enumerable: true,
    get: function() {
        return _system.path;
    }
});


var _argv = null;
Object.defineProperty(exports, 'argv', {
    configurable: false,
    enumerable: true,
    get: function() {
        if (_argv === null) {
            _argv = Object.freeze(_system.argv);
        }
        return _argv;
    }
});


Object.defineProperty(exports, 'args', {
    configurable: false,
    enumerable: true,
    get: function() {
        return (exports.argv || []).slice(1);
    }
});


exports.stdin  = io.fdopen(os.STDIN_FILENO, 'r', '<stdin>');
exports.stdout = io.fdopen(os.STDOUT_FILENO, 'w', '<stdout>');
exports.stderr = io.fdopen(os.STDERR_FILENO, 'w', '<stderr>');

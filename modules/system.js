'use strict';

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


const frozenProps = ['argv', 'build', 'env', 'versions'];

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

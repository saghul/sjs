'use strict';

const assert = require('assert');
const outil  = require('utils').object;
const sys    = require('system');
const time   = require('time');


function Console(stdout, stderr) {
    if (!(this instanceof Console)) {
        return new Console(stdout, stderr);
    }

    if (!stdout || typeof stdout.write !== 'function') {
        throw new TypeError('Console expects a writable stream instance');
    }
    if (!stderr) {
        stderr = stdout;
    } else if (typeof stderr.write !== 'function') {
        throw new TypeError('Console expects writable stream instances');
    }

    var prop = {
        writable: true,
        enumerable: false,
        configurable: true
    };
    prop.value = stdout;
    Object.defineProperty(this, '_stdout', prop);
    prop.value = stderr;
    Object.defineProperty(this, '_stderr', prop);
    prop.value = new Map();
    Object.defineProperty(this, '_times', prop);

    // bind the prototype functions to this Console instance
    var keys = Object.keys(Console.prototype);
    for (var v = 0; v < keys.length; v++) {
        var k = keys[v];
        this[k] = this[k].bind(this);
    }
}


Console.prototype.log = function() {
    this._stdout.write(outil.format.apply(null, arguments) + '\n');
};


Console.prototype.info = Console.prototype.log;


Console.prototype.warn = function() {
    this._stderr.write(outil.format.apply(null, arguments) + '\n');
};


Console.prototype.error = Console.prototype.warn;


Console.prototype.dir = function(object, options) {
    options = Object.assign({customInspect: false}, options);
    this._stdout.write(outil.inspect(object, options) + '\n');
};


Console.prototype.time = function(label) {
    this._times.set(label, performance.now());
};


Console.prototype.timeEnd = function(label) {
    const t = this._times.get(label);
    if (!t) {
        return;
    }
    const duration = performance.now() - t;
    this.log('%s: %sms', label, duration.toFixed(3));
    this._times.delete(label);
};


Console.prototype.trace = function() {
    var err = new Error();
    err.name = 'Trace';
    err.message = outil.format.apply(null, arguments);

    // remove entry for this very function
    var tmpStack = err.stack.split('\n');
    tmpStack.splice(1, 1);
    err.stack = tmpStack.join('\n');

    this.error(err.stack);
};


Console.prototype.assert = function(expression) {
    if (!expression) {
		const argsLen = arguments.length || 1;
		const arr = new Array(argsLen - 1);
		for (var i = 1; i < argsLen; i++) {
		    arr[i - 1] = arguments[i];
	    }
	    assert.ok(false, outil.format.apply(null, arr));
    }
};


var dummy = function() {};
var defaultConsole = new Console(sys.stdout, sys.stderr);
var handler = {
    get: function(target, name) {
        var value = target[name];
        return typeof value==='function' ? value : dummy;
    }
};

module.exports = new Proxy(defaultConsole, handler);
module.exports.Console = Console;

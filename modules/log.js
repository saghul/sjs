'use strict';

require('es6');

var Logger = Duktape.Logger;
var loggers = new Map();
var defaultLogger = getLogger('sjs');


Logger.prototype.setLevel = function(level) {
    this.l = level;
}


function getLogger(name) {
    var l = loggers.get(name);
    if (l === undefined) {
        l = new Logger(name);
        loggers.set(name, l);
    }
    return l;
}


module.exports.defaultLogger = defaultLogger;
module.exports.getLogger = getLogger;
module.exports.trace = Logger.prototype.trace.bind(defaultLogger);
module.exports.debug = Logger.prototype.debug.bind(defaultLogger);
module.exports.info = Logger.prototype.info.bind(defaultLogger);
module.exports.warn = Logger.prototype.warn.bind(defaultLogger);
module.exports.error = Logger.prototype.error.bind(defaultLogger);
module.exports.fatal = Logger.prototype.fatal.bind(defaultLogger);

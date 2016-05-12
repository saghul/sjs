'use strict';

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


exports.defaultLogger = defaultLogger;
exports.getLogger     = getLogger;
exports.trace         = Logger.prototype.trace.bind(defaultLogger);
exports.debug         = Logger.prototype.debug.bind(defaultLogger);
exports.info          = Logger.prototype.info.bind(defaultLogger);
exports.warn          = Logger.prototype.warn.bind(defaultLogger);
exports.error         = Logger.prototype.error.bind(defaultLogger);
exports.fatal         = Logger.prototype.fatal.bind(defaultLogger);

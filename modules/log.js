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


module.exports = {
    defaultLogger: defaultLogger,
    getLogger: getLogger,
    trace: Logger.prototype.trace.bind(defaultLogger),
    debug: Logger.prototype.debug.bind(defaultLogger),
    info: Logger.prototype.info.bind(defaultLogger),
    warn: Logger.prototype.warn.bind(defaultLogger),
    error: Logger.prototype.error.bind(defaultLogger),
    fatal: Logger.prototype.fatal.bind(defaultLogger)
};

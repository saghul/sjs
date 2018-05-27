global.console = require('console');
global.system = require('system');

// These are not supported yet, but babel-polyfill injects some.
delete global.Promise;
delete global.setTimeout;
delete global.clearTimeout;
delete global.setImmediate;
delete global.clearImmediate;

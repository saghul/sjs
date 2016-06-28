'use strict';

const sys = require('system');


sys.stdout.writeLine(JSON.stringify(sys.argv.slice(2)));

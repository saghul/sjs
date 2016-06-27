'use strict';

const io = require('io');
const sys = require('system');


io.stdout.writeLine(JSON.stringify(sys.argv.slice(2)));

'use strict';

const io = require('io');
const sys = require('system');


io.stdout.write(JSON.stringify(sys.argv.slice(2)) + '\n');

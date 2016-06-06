'use strict';

const codecs = require('codecs');
const io = require('io');
const sys = require('system');


io.stdout.write(JSON.stringify(sys.argv.slice(2)) + '\n');
io.stdout.flush();

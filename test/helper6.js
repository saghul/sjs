'use strict';

const codecs = require('codecs');
const io = require('io');
const os = require('os');
const sys = require('system');


io.stdout.write(JSON.stringify(sys.argv.slice(2)));
io.stdout.flush();

os.exit(0);

'use strict';

const codecs = require('codecs');
const io = require('io');
const os = require('os');
const sys = require('system');


io.stdout.write(JSON.stringify(sys.env));
io.stdout.flush();

os.exit(0);

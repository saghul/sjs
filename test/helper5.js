'use strict';

const codecs = require('codecs');
const io = require('io');
const sys = require('system');


io.stdout.write(JSON.stringify(sys.env));
io.stdout.flush();
